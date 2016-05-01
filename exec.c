#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

//#define w(x)	{cprintf("name: %s\n", #x);}
#define w(x)	;

int
exec(char *path, char **argv)
{
	char *s, *last;
	int i, off;
	uint argc, sz, sp, ustack[3+MAXARG+1];
	struct elfhdr elf;
	struct inode *ip;
	struct proghdr ph;
	pde_t *pgdir, *oldpgdir;
	createSwapFile(proc);
	for (i = 0; i < MAX_PSYC_PAGES; i++){
		proc->pages.disk.pageTables[i].virtualAddress = 0;
		proc->pages.disk.pageTables[i].used = 0;
		proc->pages.disk.pageTables[i].age = 0;
		proc->pages.disk.pageTables[i].ctime = 0;
		proc->pages.memory.pageTables[i].virtualAddress = 0;
		proc->pages.memory.pageTables[i].used = 0;
		proc->pages.memory.pageTables[i].age = 0;
		proc->pages.memory.pageTables[i].ctime = 0;
	}
	proc->pages.disk.count = 0;
	proc->pages.memory.count = 0;
	proc->pages.pageFaults = 0;
	proc->pages.totalPagedOut = 0;
	proc->pages.disk.lastEnterTime = 0;
	proc->pages.memory.lastEnterTime = 0;

	i = 0;
	begin_op();
	if((ip = namei(path)) == 0){
		end_op();
		return -1;
	}
	ilock(ip);
	pgdir = 0;

	// Check ELF header
	if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
		goto bad;
	if(elf.magic != ELF_MAGIC)
		goto bad;

	if((pgdir = setupkvm()) == 0)
		goto bad;

	// Load program into memory.
	sz = 0;
	for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
		if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
			goto bad;
		if(ph.type != ELF_PROG_LOAD)
			continue;
		if(ph.memsz < ph.filesz)
			goto bad;
		w(exec1)
		if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
			goto bad;
		w(exec2)
		if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
			goto bad;
	}
	iunlockput(ip);
	end_op();
	ip = 0;

	// Allocate two pages at the next page boundary.
	// Make the first inaccessible.  Use the second as the user stack.
	sz = PGROUNDUP(sz);
	w(exec3)
	if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
		goto bad;
	w(exec4)
	clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
	sp = sz;

	// Push argument strings, prepare rest of stack in ustack.
	for(argc = 0; argv[argc]; argc++) {
		if(argc >= MAXARG)
			goto bad;
		sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
		if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
			goto bad;
		ustack[3+argc] = sp;
	}
	ustack[3+argc] = 0;

	ustack[0] = 0xffffffff;  // fake return PC
	ustack[1] = argc;
	ustack[2] = sp - (argc+1)*4;  // argv pointer

	sp -= (3+argc+1) * 4;
	if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
		goto bad;

	// Save program name for debugging.
	for(last=s=path; *s; s++)
		if(*s == '/')
			last = s+1;
	safestrcpy(proc->name, last, sizeof(proc->name));

	// Commit to the user image.
	oldpgdir = proc->pgdir;
	proc->pgdir = pgdir;
	proc->sz = sz;
	proc->tf->eip = elf.entry;  // main
	proc->tf->esp = sp;
	switchuvm(proc);
	freevm(oldpgdir);
	return 0;

	bad:
	w(exec5)
	if(pgdir)
		freevm(pgdir);
	if(ip){
		iunlockput(ip);
		end_op();
	}
	return -1;
}

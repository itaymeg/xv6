.globl sigint
sigint:
movl $22,%eax
int $0x40
ret
.globl sigintend
sigintend:

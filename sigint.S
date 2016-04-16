.globl sigint
sigint:
movl $22,%eax
int $0x40
.globl sigintend
sigintend:

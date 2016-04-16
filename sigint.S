.global sigint
.global sigintend
sigint:
movl $22,%eax
int $T_SYSCALL
sigintend:

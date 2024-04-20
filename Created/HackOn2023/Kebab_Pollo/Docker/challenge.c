#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <seccomp.h>

int main(){

    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);

	char * buf = (char*)mmap((void *)0xdead0000, 0x1000, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANON | MAP_FIXED | MAP_PRIVATE, -1, 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_load(ctx);
	read(0, buf, 5);
	__asm__("mov rax, %0;"
			"xor rsi, rsi;"
			"call rax;"
			:
			: "r" (buf)
			: "rax", "rsi", "memory"
			);

    seccomp_release(ctx);

	return 0;
}

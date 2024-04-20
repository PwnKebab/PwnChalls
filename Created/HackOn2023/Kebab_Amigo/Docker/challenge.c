#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <stdint.h>
#include <seccomp.h>


void setup(){
   setvbuf(stdout, NULL, _IONBF, 0);
   setvbuf(stdin, NULL, _IONBF, 0);
   setvbuf(stderr, NULL, _IONBF, 0);
}

typedef struct chall{
	long long string0;
	long long string1;
	long long string2;
	char string3;
	char buff[200];
	void *ptr;
}chall_t;

void * _;
int main(int argc, char const *argv[])
{
   setup();
   scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);
   seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0);
   seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
   seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
   seccomp_load(ctx);
	chall_t mychall;
	mychall.string0 = 0x696e65766e656942;
	mychall.string1 = 0x62654b2061206f64;
	mychall.string2 = 0x216f67696d416261;
	mychall.string3 = 0;
	mychall.buff[200];
	mychall.ptr;
	_ = (void *)&mychall.string0;
	mychall.ptr = _;
	for (int i = 0; i < 3; ++i)
	{
		gets(mychall.buff);
		puts((char*)mychall.ptr);
	}
	return 0;
}

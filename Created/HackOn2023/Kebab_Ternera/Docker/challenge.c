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

char xor(char* buffer, uint size);
void shellcode ();
void setup();
void welcome();

char xor(char* buffer, uint size){

   char out = 0;

   for (uint i = 0; i < size; i++){
     out ^= buffer[i];
   }

   return out;
}

void shellcode (){

   size_t shellcodeSize;
   char buffer[1024];
   shellcodeSize = read(0, buffer, 1024);
   void (*ex)();
   void *ptr = mmap(NULL, shellcodeSize, PROT_WRITE | PROT_EXEC | PROT_READ, MAP_PRIVATE | MAP_ANON, -1, 0);
   memcpy(ptr, buffer, shellcodeSize);
   ex = (void (*)()) ptr;

   if (shellcodeSize < 1024){
      exit(1);
   }

   for (int i=0; i<shellcodeSize; i++){

        uint16_t *sc = (uint16_t *)((uint8_t*)ptr + i);
        if (*sc == 0x80cd || *sc == 0x340f || *sc == 0x050f || ((uint8_t*)ptr)[i] == 'H' || !((uint8_t*)ptr)[i] || ((uint8_t*)ptr)[i] == 0x90){
            exit(1);
        }
   }

   char mitad1 = xor(buffer, 512);
   char mitad2 = xor(buffer + 512, 512);

   if (mitad1 != mitad2){
      exit(1);
   }

   scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);
   seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0);
   seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
   seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
   seccomp_load(ctx);
   seccomp_release(ctx);
   
   ex();

   return;
}

void setup(){
   setvbuf(stdout, NULL, _IONBF, 0);
   setvbuf(stdin, NULL, _IONBF, 0);
   setvbuf(stderr, NULL, _IONBF, 0);
}

void welcome(){
   printf("[*] Bienvenido [*]\n");
   shellcode();
}

int main(int argc, char* argv[]){

   setup();

   welcome();

   return 0;
}

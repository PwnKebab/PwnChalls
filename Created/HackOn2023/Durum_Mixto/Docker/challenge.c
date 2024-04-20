#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct chall{
	char filebuff[1024];
	char str1[1024];
	char str2[1024];
}chall_t;

chall_t mychall;

void pMenu(){
	puts("[1] Leer un archivo TUYO");
	puts("[2] Leer str1 de stdin");
	puts("[3] Copiar str1 a str2"); // rep movs normal
	puts("[4] Copiar str2 a str1"); // rep movs al reves (std)
	puts("[5] Escribir str1 a stdout");
	puts("[s] Salir");
}

void readOwnedFile(){
	struct stat statbuf;
	char filename[1024];
	printf("File: \n");
	fgets(filename, 1024, stdin);
	char * n = strchr(filename, '\n');
	n? (*n = 0):(mychall.str1[1023] = 0,getchar());
	stat(filename, &statbuf); 
	if(statbuf.st_uid == getuid()){ 
		FILE * f = fopen(filename, "r");
		fgets(mychall.filebuff, 1024, f);
		fclose(f);
		return;
	}
	printf("Error: acceso no permitido\n");
	exit(1);
}

void readStr1(){
	fgets(mychall.str1, 1024, stdin);
	char * n = strchr(mychall.str1, '\n');
	n? (*n = 0):(mychall.str1[1023] = 0,getchar());
}

void str1tostr2(){
	__asm__ goto("test %0, %0;"
			"jz %l3;"
			"xor rcx, rcx;"
			"mov ecx, %0;"
			"lea rdi, %1;"
			"lea rsi, %2;"
			"rep movsb;"
			: 
			: "r" ((int)strlen(mychall.str1)), "m" (mychall.str2), "m" (mychall.str1)
			: "rcx", "rdi", "rsi", "memory"
			: end
			);
	end:
	return;
}

void str2tostr1(){
	__asm__ goto("std;"
			"test %0, %0;"
			"jz %l3;"
			"mov rcx, %0;"
			"sub %0, 1;"
			"lea rdi, %1;"
			"add rdi, %0;"
			"lea rsi, %2;"
			"add rsi, %0;"
			"rep movsb;"
			"cld;"
			: 
			: "r" ((long)strlen(mychall.str2)), "m" (mychall.str1), "m" (mychall.str2)
			: "rcx", "rdi", "rsi", "memory"
			: end
			);
	end:
	return;
}

int main(int argc, char const *argv[])
{
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	while(1){
		pMenu();
		unsigned char option = (unsigned char) getchar();
		getchar();
		switch(option){
			case '1':	readOwnedFile();break;
			case '2':	readStr1();break;
			case '3':	str1tostr2();break;
			case '4':	str2tostr1();break;
			case '5':	puts(mychall.str1);break;
			case 's': 	exit(0);
			default: 	puts("[!] Opcion incorrecta");
		}
	}

	return 0;
}

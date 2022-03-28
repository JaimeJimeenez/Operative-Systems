#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int my_system(char* const argv) {
	pid_t pid;
	int status;
	pid = fork();

	switch(pid) {
		case -1:
			exit(-1);
		case 0:
			execl("/bin/bash", "bash", "-c", argv, NULL);
			exit(1);
		default:
			wait(&status);
	}

	return status;
}

int main(int argc, char* argv[])
{
	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	return my_system(argv[1]);
}


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<linux/string.h>

void mysys(char *command)
{
	char com[100];
	strcpy(com,command);
	pid_t pid;
	pid=fork();
	char* argv[10];
	char* s;
	int count=0;
	if(pid==0)
	{
		s=strtok(com," ");
		argv[0]=s;
		while(s!=NULL)
		{
			count++;
			s=strtok(NULL," ");
			argv[count]=s;
		}
		argv[count]=NULL;
		int error=execvp(argv[0],argv);
		if(error<0)
			perror("execvp");
	}
	wait(NULL);
}

int main()
{
	printf("-------------------------------------------------------\n");
	mysys("echo HELLO WORLD");
	printf("-------------------------------------------------------\n");
	mysys("ls /");
	printf("-------------------------------------------------------\n");
	return 0;
}

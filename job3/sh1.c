#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int argc;
char *argv[16];

void split(char *command)
{
	char *com=(char*)malloc(sizeof(command));
	strcpy(com,command);
	argc=0;
	for(int i=0;i<16;i++)
		argv[i]=NULL;
	char *s;
	s=strtok(com," \n");
	while(s!=NULL)
	{
		argv[argc++]=s;
		s=strtok(NULL," \n");
	}
}

void build_in()
{
	if(strcmp(argv[0],"cd")==0)
	{
		int error=chdir(argv[1]);
		if(error<0)
			perror("cd");
		else
		{
			char *path=getcwd(NULL,0);
			printf("working dir:%s\n",path);
			free(path);
		}
	}
	else if(strcmp(argv[0],"pwd")==0)
	{
		char *path=getcwd(NULL,0);
		printf("working dir:%s\n",path);
		free(path);
	}
	else if(strcmp(argv[0],"exit")==0)
		exit(0);
}

void mysys(char *command)
{
	split(command);
	if(strcmp(argv[0],"cd")==0||strcmp(argv[0],"pwd")==0||strcmp(argv[0],"exit")==0)
	{
		build_in();
		return ;
	}
	pid_t pid;
	pid=fork();
	if(pid==0)
	{
		int error=execvp(argv[0],argv);
		if(error<0)
			perror("execvp");
	}
	wait(NULL);
}
int main()
{
	char command[256];
	int i;
	while(1)
	{
		write(1,">",sizeof(">"));
		i=read(0,command,sizeof(command));
		command[i]=0;
		mysys(command);
	}
	return 0;
}



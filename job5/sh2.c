#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/stat.h>

int argc;
char *argv[16];
char *input;
char *output;
int outmode;

void split(char *command)
{
	outmode=0;
	input=NULL;
	output=NULL;
	char *com=(char*)malloc(sizeof(command));
	strcpy(com,command);
	argc=0;
	for(int i=0;i<16;i++)
		argv[i]=NULL;
	char *s;
	s=strtok(com," \n");
	while(s!=NULL)
	{
		char *in=strchr(s,'<');
		char *out=strchr(s,'>');
		if(in)
			input=in+1;
		else if(out)
		{
			if(out[1]=='>')
			{
				outmode=1;
				output=out+2;
			}
			else
				output=out+1;
			
		}
		else
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

void redirect()
{
	int fd;
	if(input)
	{
		fd=open(input,O_RDONLY);
		if(fd==-1)
		{
			printf("Open Fail\n");
			exit(0);
		}
		dup2(fd,0);
		close(fd);
	}
	if(output)
	{
		if(outmode==0)
			fd=open(output,O_CREAT|O_TRUNC|O_WRONLY,0666);
		else
			fd=open(output,O_CREAT|O_APPEND|O_WRONLY,0666);
		if(fd==-1)
		{
			printf("Open Fail\n");
			exit(0);
		}
		dup2(fd,1);
		close(fd);
	}
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
		if(input||output)
			redirect();
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



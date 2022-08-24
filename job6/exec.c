#include "exec.h"

void tree_execute_redirect(tree_t* root)
{
	int fd;
	if(root->childv[1]->token=="<")
	{
		fd=open(root->childv[2]->token.c_str(),O_RDONLY,0666);
		dup2(fd,0);
	}
	else if(root->childv[1]->token=="<<")
	{
		fd=open(root->childv[2]->token.c_str(),O_RDONLY,0666);
		dup2(fd,0);
	}
	else if(root->childv[1]->token==">")
	{
		fd=open(root->childv[2]->token.c_str(),O_CREAT|O_RDWR|O_APPEND,0666);
		dup2(fd,1);
	}
	else if(root->childv[1]->token==">>")
	{
		fd=open(root->childv[2]->token.c_str(),O_CREAT|O_TRUNC|O_RDWR,0666);
		dup2(fd,1);
	}
	tree_execute_wrapper(root->childv[0]);
}

void tree_execute_basic(tree_t* root)
{
	char** argv=(char**)malloc(32*sizeof(char*));
	int i;
	for(i=0;i<root->childv.size();i++)
	{
		argv[i]=(char*)malloc(32*sizeof(char));
		strcpy(argv[i],root->childv[i]->token.c_str());
	}
	argv[i]=nullptr;
	execvp(*argv,argv);
}

void tree_execute_pipe(tree_t* root)
{
	int fd[2];
	pipe(fd);
	pid_t pid=fork();
	if(pid==0)
	{
		dup2(fd[1],1);
		close(fd[0]);
		close(fd[1]);
		tree_execute_wrapper(root->childv[0]);
		exit(0);
	}
	wait(nullptr);
	dup2(fd[0],0);
	close(fd[0]);
	close(fd[1]);
	tree_execute_wrapper(root->childv[1]);
}

bool tree_execute_builtin(tree_t* root)
{
	if(root->type==TREE_BASIC)
	{
		string token=root->childv[0]->token;
		if(token=="cd")
		{
			chdir(root->childv[1]->token.c_str());
			return true;
		}
		else if(token=="exit")
		{
			exit(0);
		}
	}
	return false;
}

void tree_execute_async(tree_t* root)
{
	tree_execute_wrapper(root->childv[0]);
}

void tree_execute(tree_t* root)
{
	switch(root->type)
	{
		case TREE_ASYNC:
			tree_execute_async(root);
			break;
		case TREE_PIPE:
			tree_execute_pipe(root);
			break;
		case TREE_REDIRECT:
			tree_execute_redirect(root);
			break;
		case TREE_BASIC:
			tree_execute_basic(root);
			break;
	}
}

void tree_execute_wrapper(tree_t* root)
{
	if(tree_execute_builtin(root)) return;
	int status;
	pid_t pid=fork();
	if(pid==0)
	{
		tree_execute(root);
		exit(0);
	}
	if(root->type!=TREE_ASYNC)
		wait(&status);
}

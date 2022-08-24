#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc,char *argv[])
{
	DIR  *dir;
	char path[128];
	if(argc==1)
	{
		getcwd(path,sizeof(path));
		dir=opendir(path);
	}
	else
	{
		char *s=argv[1];
		if(s[0]=='/')
			dir=opendir(argv[1]);
		else
		{
			getcwd(path,sizeof(path));
			strcat(path,"/");
			strcat(path,argv[1]);
			dir=opendir(path);
		}
	}
	while(1)
	{
		struct dirent *enter=readdir(dir);
		if(enter==NULL)
			break;
		if(strcmp(enter->d_name,".")!=0&&strcmp(enter->d_name,"..")!=0)
			printf("%s\n",enter->d_name);
	}
	closedir(dir);
	return 0;
}

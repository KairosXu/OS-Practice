#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<unistd.h>

void tree(DIR *dir,int depth,char *path)
{
	if(dir==NULL)
		return;
	while(1)
	{
		struct dirent *enter=readdir(dir);
		if(enter==NULL)
			return;
		if(strcmp(enter->d_name,".")==0||strcmp(enter->d_name,"..")==0)
			continue;
		if(enter->d_type==DT_DIR)
		{
			char temp[128];
			strcpy(temp,path);
			for(int i=0;i<depth;i++)
				printf(" ");
			printf("%s\n",enter->d_name);
			strcat(temp,"/");
			strcat(temp,enter->d_name);
			DIR *dirptr=opendir(temp);
			tree(dirptr,depth+1,temp);
			closedir(dirptr);
		}
		else
		{
			for(int i=0;i<depth;i++)
				printf(" ");
			printf("%s\n",enter->d_name);
		}
	}
}

int main(int argc,char *argv[])
{
	DIR *dir;
	char path[128];
	if(argc==1)
	{
		getcwd(path,sizeof(path));
		dir=opendir(path);
		printf(".\n");
	}
	else
	{
		char *s=argv[1];
		if(s[0]=='/')
		{
			strcpy(path,argv[1]);
			dir=opendir(argv[1]);
			printf("%s\n",argv[1]);
		}
		else
		{
			printf("%s\n",argv[1]);
			getcwd(path,sizeof(path));
			strcat(path,"/");
			strcat(path,argv[1]);
			dir=opendir(path);
		}
	}
	tree(dir,1,path);
	closedir(dir);
	return 0;
}

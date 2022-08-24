#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		printf("Error!\n");
		exit(0);
	}
	int fd;
	char* buffer;
	size_t count;
	fd=open(argv[1],O_RDONLY);
	if(fd<0)
	{
		printf("Open Error!\n");
		exit(0);
	}
	struct stat s;
	stat(argv[1],&s);
	count=s.st_size;
	buffer=(char*)malloc((count+1)*sizeof(char));
	if(!buffer)
	{
		printf("Memory Error!\n");
		exit(0);
	}
	read(fd,buffer,count);
	close(fd);
	fd=open(argv[2],O_WRONLY|O_CREAT);
	if(fd<0)
	{
		printf("Open Error!\n");
		exit(0);
	}
	write(fd,buffer,count);
	close(fd);
	free(buffer);
	return 0;
}

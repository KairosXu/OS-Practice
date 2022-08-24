#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		printf("Error!\n");
		exit(0);
	}
	int fd;
	char *buffer;
	size_t count;
	struct stat s;
	fd=open(argv[1],O_RDONLY);
	if(fd<0)
	{
		printf("Open Error!\n");
		exit(0);
	}
	stat(argv[1],&s);
	count=s.st_size;
	buffer=(char*)malloc((count+1)*sizeof(char));
	if(!buffer)
	{
		printf("Memory Error!\n");
		exit(0);
	}
	read(fd,buffer,count);
	write(1,buffer,count);
	close(fd);
	free(buffer);
	return 0;
}

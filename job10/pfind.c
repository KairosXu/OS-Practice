#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pthread.h>

#define WORKER_NUMBER 4
#define TASK_COUNT 128

struct task{
	int is_end;
	char path[128];
	char string[128];
};

struct taskqueue{
	struct task task;
}task_queue[TASK_COUNT];

int in;
int out;

void put_task_queue(char *path,char *string,int is_end)
{
	strcpy(task_queue[in].task.path,path);
	strcpy(task_queue[in].task.string,string);
	task_queue[in].task.is_end=is_end;
	in=(in+1)%TASK_COUNT;
}

struct task get_task_queue()
{
	struct task task;
	task=task_queue[out].task;
	out=(out+1)%TASK_COUNT;
	return task;
}

typedef struct
{
	int value;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} sema_t;

void sema_init(sema_t *sema,int value)
{
	sema->value=value;
	pthread_mutex_init(&sema->mutex,NULL);
	pthread_cond_init(&sema->cond,NULL);
}

void sema_wait(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	while(sema->value<=0)
		pthread_cond_wait(&sema->cond,&sema->mutex);
	sema->value--;
	pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	++sema->value;
	pthread_cond_signal(&sema->cond);
	pthread_mutex_unlock(&sema->mutex);
}

sema_t mutex_sema;
sema_t empty_queue_sema;
sema_t full_queue_sema;

void find_file(char *path,char *target)
{
	FILE *file=fopen(path,"r");

	char line[256];
	while(fgets(line,sizeof(line),file))
	{
		if(strstr(line,target))
			printf("%s: %s",path,line);
	}

	fclose(file);
}

void produce(char *path,char *string,int is_end)
{
	sema_wait(&empty_queue_sema);
	sema_wait(&mutex_sema);

	put_task_queue(path,string,is_end);

	sema_signal(&mutex_sema);
	sema_signal(&full_queue_sema);
}

void *worker_entry(void *arg)
{
	while(1)
	{
		struct task task;

		sema_wait(&full_queue_sema);
		sema_wait(&mutex_sema);

		task=get_task_queue();
		
		if(task.is_end==0)
			find_file(task.path,task.string);
		
		sema_signal(&mutex_sema);

		if(task.is_end==1)	
			break;

		sema_signal(&empty_queue_sema);
	}
}

void find_dir(char *path,char *target)
{
	DIR* dir=opendir(path);
	struct dirent* entry;
	while(entry=readdir(dir))
	{
		char temp[256]={0};
		strcpy(temp,path);
		temp[strlen(path)]='/';
		temp[strlen(path)+1]='\0';
		strcat(temp,entry->d_name);

		if(strcmp(entry->d_name,".")==0)
			continue;

		if(strcmp(entry->d_name,"..")==0)
			continue;

		if(entry->d_type==DT_DIR)
			find_dir(temp,target);

		if(entry->d_type==DT_REG)
			produce(temp,target,0);
	}
	closedir(dir);
}

int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		puts("Usage: sfind file string");
		return 0;
	}

	char *path=argv[1];
	char *string=argv[2];

	struct stat info;
	stat(path,&info);

	pthread_t worker_tid[WORKER_NUMBER];
	
	sema_init(&mutex_sema,1);
	sema_init(&empty_queue_sema,TASK_COUNT-1);
	sema_init(&full_queue_sema,0);

	for(int i=0;i<WORKER_NUMBER;i++)
	{
		pthread_create(&worker_tid[i],NULL,worker_entry,NULL);
	}

	if(S_ISDIR(info.st_mode))
	{
		find_dir(path,string);
	}
	else
	{
		find_file(path,string);
		return 0;
	}
	for(int i=0;i<WORKER_NUMBER;i++)
	{
		produce("task_end","task_end",1);
	}

	for(int i=0;i<WORKER_NUMBER;i++)
	{
		pthread_join(worker_tid[i],NULL);
	}

	return 0;
}

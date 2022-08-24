#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

#define N 10

int state;

pthread_mutex_t mutex;
pthread_cond_t wait_ping;
pthread_cond_t wait_pong;

void *ping(void *arg)
{	
	for(int i=0;i<N;i++)
	{
		pthread_mutex_lock(&mutex);
		while(state==1)
			pthread_cond_wait(&wait_ping,&mutex);
		printf("ping\n");
		state=1;
		pthread_cond_signal(&wait_pong);
		pthread_mutex_unlock(&mutex);
	}
}

void *pong(void *arg)
{	
	for(int i=0;i<N;i++)
	{
		pthread_mutex_lock(&mutex);
		while(state==0)
			pthread_cond_wait(&wait_pong,&mutex);
		printf("pong\n");
		state=0;
		pthread_cond_signal(&wait_ping);
		pthread_mutex_unlock(&mutex);
	}
}

int main()
{
	pthread_t pong_tid;
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&wait_ping,NULL);
	pthread_cond_init(&wait_pong,NULL);

	pthread_create(&pong_tid,NULL,pong,NULL);
	ping(NULL);
	pthread_join(pong_tid,NULL);
	return 0;
}

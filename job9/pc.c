#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

#define CAPACITY 4
#define ITEM_COUNT 8

int buffer1[CAPACITY],buffer2[CAPACITY];
int in1;
int out1;
int in2;
int out2;

int buffer1_is_empty()
{
	return in1==out1;
}

int buffer1_is_full()
{
	return (in1+1)%CAPACITY==out1;
}

int buffer2_is_empty()
{
	return in2==out2;
}

int buffer2_is_full()
{
	return (in2+1)%CAPACITY==out2;
}

void put_item_buffer1(int item)
{
	buffer1[in1]=item;
	in1=(in1+1)%CAPACITY;
}

int get_item_buffer1()
{
	int item;
	item=buffer1[out1];
	out1=(out1+1)%CAPACITY;
	return item;
}

void put_item_buffer2(int item)
{
	buffer2[in2]=item;
	in2=(in2+1)%CAPACITY;
}

int get_item_buffer2()
{
	int item;
	item=buffer2[out2];
	out2=(out2+1)%CAPACITY;
	return item;
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

sema_t mutex1_sema;
sema_t mutex2_sema;
sema_t empty_buffer1_sema;
sema_t full_buffer1_sema;
sema_t empty_buffer2_sema;
sema_t full_buffer2_sema;

void *produce(void *arg)
{
	int i;
	int item;
	for(i=0;i<ITEM_COUNT;i++)
	{
		sema_wait(&empty_buffer1_sema);
		sema_wait(&mutex1_sema);
		
		item='a'+i;
		put_item_buffer1(item);
		printf("%c\n",item);

		sema_signal(&mutex1_sema);
		sema_signal(&full_buffer1_sema);
	}
}

void *calculate(void *arg)
{
	int i;
	int item;
	for(i=0;i<ITEM_COUNT;i++)
	{	
		sema_wait(&full_buffer1_sema);
		sema_wait(&mutex1_sema);

		item=get_item_buffer1();
		//printf("%c\n",item);

		sema_signal(&mutex1_sema);
		sema_signal(&empty_buffer1_sema);

		sema_wait(&empty_buffer2_sema);
		sema_wait(&mutex2_sema);
		
		item-=32;
		put_item_buffer2(item);
		printf("	%c:%c\n",item+32,item);

		sema_signal(&mutex2_sema);
		sema_signal(&full_buffer2_sema);
	}
}

void *consume(void *arg)
{
	int i;
	int item;
	
	for(i=0;i<ITEM_COUNT;i++)
	{
		sema_wait(&full_buffer2_sema);
		sema_wait(&mutex2_sema);

		item=get_item_buffer2();
		printf("		%c\n",item);

		sema_signal(&mutex2_sema);
		sema_signal(&empty_buffer2_sema);
	}
}

int main()
{
	pthread_t consumer_tid;
	pthread_t calculator_tid;

	sema_init(&mutex1_sema,1);
	sema_init(&mutex2_sema,1);
	sema_init(&empty_buffer1_sema,CAPACITY-1);
	sema_init(&full_buffer1_sema,0);
	sema_init(&empty_buffer2_sema,CAPACITY-1);
	sema_init(&full_buffer2_sema,0);
	
	pthread_create(&calculator_tid,NULL,calculate,NULL);
	pthread_create(&consumer_tid,NULL,consume,NULL);
	produce(NULL);
	pthread_join(calculator_tid,NULL);
	pthread_join(consumer_tid,NULL);
	
	return 0;
}

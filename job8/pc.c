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

pthread_mutex_t mutex_1;
pthread_mutex_t mutex_2;
pthread_cond_t wait_empty_buffer1;
pthread_cond_t wait_full_buffer1;
pthread_cond_t wait_empty_buffer2;
pthread_cond_t wait_full_buffer2;

void *produce(void *arg)
{
	int i;
	int item;
	for(i=0;i<ITEM_COUNT;i++)
	{
		pthread_mutex_lock(&mutex_1);
		while(buffer1_is_full())
			pthread_cond_wait(&wait_empty_buffer1,&mutex_1);
		
		item='a'+i;
		put_item_buffer1(item);
		printf("%c\n",item);

		pthread_cond_signal(&wait_full_buffer1);
		pthread_mutex_unlock(&mutex_1);
	}
}

void *calculate(void *arg)
{
	int i;
	int item;
	for(i=0;i<ITEM_COUNT;i++)
	{	
		pthread_mutex_lock(&mutex_1);
		while(buffer1_is_empty())
			pthread_cond_wait(&wait_full_buffer1,&mutex_1);

		item=get_item_buffer1();
		//printf("%c\n",item);

		pthread_cond_signal(&wait_empty_buffer1);
		pthread_mutex_unlock(&mutex_1);

		pthread_mutex_lock(&mutex_2);
		while(buffer2_is_full())
			pthread_cond_wait(&wait_empty_buffer2,&mutex_2);
		
		item-=32;
		put_item_buffer2(item);
		printf("	%c:%c\n",item+32,item);

		pthread_cond_signal(&wait_full_buffer2);
		pthread_mutex_unlock(&mutex_2);
	}
}

void *consume(void *arg)
{
	int i;
	int item;
	
	for(i=0;i<ITEM_COUNT;i++)
	{
		pthread_mutex_lock(&mutex_2);
		while(buffer2_is_empty())
			pthread_cond_wait(&wait_full_buffer2,&mutex_2);

		item=get_item_buffer2();
		printf("		%c\n",item);

		pthread_cond_signal(&wait_empty_buffer2);
		pthread_mutex_unlock(&mutex_2);
	}
}

int main()
{
	pthread_t consumer_tid;
	pthread_t calculator_tid;
	pthread_mutex_init(&mutex_1,NULL);
	pthread_mutex_init(&mutex_2,NULL);
	pthread_cond_init(&wait_empty_buffer1,NULL);
	pthread_cond_init(&wait_full_buffer1,NULL);
	pthread_cond_init(&wait_empty_buffer2,NULL);
	pthread_cond_init(&wait_full_buffer2,NULL);
	
	pthread_create(&calculator_tid,NULL,calculate,NULL);
	pthread_create(&consumer_tid,NULL,consume,NULL);
	produce(NULL);
	pthread_join(calculator_tid,NULL);
	pthread_join(consumer_tid,NULL);
	
	return 0;
}

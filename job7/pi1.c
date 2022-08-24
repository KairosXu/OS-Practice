#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

#define N 100000

double sum1,sum2;

void *worker(void *arg)
{
	for(int i=N/2;i<N;i++)
	{
		if(i%2==0)
			sum2+=1.0/(2*i+1);
		else
			sum2-=1.0/(2*i+1);
	}
}

void master()
{	
	for(int i=0;i<N/2;i++)
	{
		if(i%2==0)
			sum1+=1.0/(2*i+1);
		else
			sum1-=1.0/(2*i+1);
	}
}

int main()
{
	pthread_t tid;
	pthread_create(&tid,NULL,&worker,NULL);
	master();
	pthread_join(tid,NULL);
	double pi=(sum1+sum2)*4;
	printf("Times:%d\n",N);
	printf("pi=%f\n",pi);
	
}

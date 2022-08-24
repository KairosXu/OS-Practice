#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

#define Times 100000
#define N 10
#define Aver Times/N

struct result
{
	double result;
};

struct param
{
	int start;
	int end;
};

void *compute(void *arg)
{
	struct param *param;
	struct result *result;
	double sum;
	param=(struct param *)arg;
	for(int i=param->start;i<param->end;i++)
	{
		if(i%2==0)
			sum+=1.0/(2*i+1);
		else
			sum-=1.0/(2*i+1);
	}
	result=malloc(sizeof(struct result));
	result->result=sum;
	return result;
}

int main()
{
	pthread_t tid[N];
	struct param params[N];
	for(int i=0;i<N;i++)
	{
		struct param *param;
		param=&params[i];
		param->start=i*Aver;
		param->end=(i+1)*Aver;
		pthread_create(&tid[i],NULL,&compute,param);
	}
	double sum=0;
	for(int i=0;i<N;i++)
	{
		struct result *result;
		pthread_join(tid[i],(void **)&result);
		sum+=result->result;
		free(result);
	}
	double pi=4*sum;
	printf("Times:%d Threads:%d\n",Times,N);
	printf("pi=%f\n",pi);
	return 0;
}

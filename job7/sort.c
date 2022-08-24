#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

int array[]={4,0,9,2,7,6,8,5,1,3};
#define N 10

struct param
{
	int *array;
	int start;
	int end;
};

void *workersort(void *arg)
{
	struct param *param;
	param=(struct param *)arg;
	for(int i=param->start;i<param->end;i++)
	{
		int k=i;
		for(int j=i+1;j<param->end;j++)
		{
			if(param->array[j]<param->array[k])
				k=j;
		}
		if(k!=i)
		{
			int temp;
			temp=param->array[i];
			param->array[i]=param->array[k];
			param->array[k]=temp;
		}
	}
}

void mastersort()
{
	int mid=N/2;
	int i=0;
	int j=mid;
	int temp[N];
	for(int k=0;k<N;k++)
	{
		temp[k]=array[k];
	}
	for(int k=0;k<N;k++)
	{
		if(i>=mid)
		{
			array[k]=temp[j];
			j++;
		}
		else if(j>=N)
		{
			array[k]=temp[i];
			i++;
		}
		else if(temp[i]>temp[j])
		{
			array[k]=temp[j];
			j++;
		}
		else
		{
			array[k]=temp[i];
			i++;
		}
	}
}

int main()
{
	for(int i=0;i<N;i++)
		printf("%d ",array[i]);
	printf("\n");
	pthread_t tid[2];
	struct param params[2];
	for(int i=0;i<2;i++)
	{
		struct param *param;
		param=&params[i];
		param->array=array;
		param->start=i*(N/2);
		param->end=(i+1)*(N/2);
		pthread_create(&tid[i],NULL,&workersort,(void *)param);
	}
	for(int i=0;i<2;i++)
	{
		pthread_join(tid[i],NULL);
	}
	mastersort();
	for(int i=0;i<N;i++)
		printf("%d ",array[i]);
	printf("\n");
	return 0;
}

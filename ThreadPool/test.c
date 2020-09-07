#include "ThreadPool.h"

int p,d,f;

void *task1(void *arg){
	printf("task 1 is running\n");
	while(p != 1){
		usleep(1);
	}
	printf("task 1 is finish\n");
}

void *task2(void *arg){
	printf("task 2 is running\n");
	char *i = (char *)arg;
	printf("%s\n",i);
	while(d != 2){
		usleep(1);
	}
	printf("task 2 is finish\n");
}

void *task3(void *arg){
	printf("task 3 is running\n");
	int *i = arg;
	int n = *i;
	printf("%d\n",n);
	while(f != 3){
		usleep(1);
	}
	printf("task 3 is finish\n");
}

void display(pthread_pool *pool){
	static int nCount = 1;
    
    printf("****************************\n");
    printf("nCount = %d\n", nCount++);
    printf("max_thread_num = %d\n", get_max_thread_num(pool));
    printf("current_thread_num = %d\n", get_current_thread_num(pool));
    printf("current_thread_task_num = %d\n", get_current_thread_task_num(pool));
    printf("current_thread_free_num = %d\n", get_current_thread_free_num(pool));
    printf("current_wait_task_num = %d\n", get_current_wait_task_num(pool));
    printf("max_free_thread_num = %d\n", get_max_free_thread_num(pool));
    printf("****************************\n");
}
int main(){
	pthread_pool *pool = NULL;
	char *m = "Hello world";
	int t = 5;
	pool = (pthread_pool *)malloc(sizeof(pthread_pool));
	int q = create_thread_pool(pool,2,2,5);
	if(q != 0){
		printf("%d\n",q);
		return -1;
	}
	display(pool);

	add_task(pool,task1,NULL);
	usleep(200);
	display(pool);
	
	add_task(pool,task2,(void *)m);
	usleep(200);
	display(pool);
	
	add_task(pool,task3,(void *)&t);
	usleep(200);
	display(pool);
	
	
	p = 1;
	usleep(200);
	display(pool);
	
	d = 2;
	usleep(200);
	display(pool);
	
	f = 3;
	usleep(200);
	display(pool);

	
	usleep(400);
	thread_pool_destroy(pool);
	return 0;
}

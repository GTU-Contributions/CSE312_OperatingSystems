#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Thread structure */
typedef struct ts{
	int start;
	int finish;
	int result;
}ts_t; 

void *threadFunc(void *arg){
	ts_t *current = (ts_t *)arg;
	int i;
	current->result = 0;	
	for(i=current->start; i<=current->finish; ++i)
		current->result = current->result + i;	
	pthread_exit(0);
}

int main(){
	int finalResult = 0;
	pthread_t one, two;
	ts_t	oneS, twoS;
	oneS.start = 1; oneS.finish = 50;
	twoS.start = 51; twoS.finish = 100;
	pthread_create(&one, NULL, threadFunc, &oneS);
	pthread_create(&two, NULL, threadFunc, &twoS);
	pthread_join(one, NULL); pthread_join(two, NULL);
	finalResult = oneS.result + twoS.result;
	printf("The result is: %d\n", finalResult);	
	return 0;
}





#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include "bbuff.h"
#include "stats.h"

typedef struct{
	int factory_number;
	double time_stamp_in_ms;
} candy_t;

_Bool stop_thread = false;

double current_time_in_ms(void){
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

void *factThread(void* num){
	int i = *(int*) num;

	while(!stop_thread){
		int delay = rand() % 3;
		printf("\tFactory %d ships candy & waits %ds\n", i, delay);

		candy_t* candy = malloc(sizeof(candy_t));
		candy -> factory_number = i;
		candy -> time_stamp_in_ms = current_time_in_ms();

		bbuff_blocking_insert(candy);
		//printf("Passed buffer");
		stats_record_produced(i);
		sleep(delay);
	}

	printf("Candy-factory %d done\n", i);
	pthread_exit(NULL);
}

void *kidThread(void* num){
	int delay;
	while(true){
		delay = rand() % 2;
		candy_t* candy = (candy_t*)bbuff_blocking_extract();

		if(candy != NULL) stats_record_consumed(candy -> factory_number, current_time_in_ms() - candy -> time_stamp_in_ms);
		
		free(candy);
		sleep(delay);
	}
}

int main(int argc, char* argv[]){

	//1. Extract arguments
	if(argc != 4){
		printf("Exactly 3 arguments are required");
		exit(1);
	}
	//printf("1.%s 2.%s 3. %s\n", argv[1], argv[2], argv[3]);
	for(int i = 1; i <= 3; i++){
		if(atoi(argv[i]) <= 0 ){
			printf("All arguments must be greater than 0");
			exit(1);
		}
	}

	int factNum = atoi(argv[1]);
	int kidNum = atoi(argv[2]);
	int secNum = atoi(argv[3]);

	//2. Initialize modules
	bbuff_init();
	stats_init(factNum);

	//3. Launch candy-factory threads
	pthread_t factThreads[factNum];
	int threadID[factNum];

	for(int i = 0; i < factNum; i++){
		threadID[i] = i;
		pthread_create(&factThreads[i], NULL, factThread, &threadID[i]);
		//printf("%d...", i);
	}

	//4. Launch kid threads
	pthread_t kidThreads[kidNum];
	for(int i = 0; i < kidNum; i++) pthread_create(&kidThreads[i], NULL, kidThread, &threadID[i]);

	//5. Wait for Requested time
	for(int i = 0; i < secNum; i++){
		sleep(1);
		printf("Time: %ds\n", i);
	}
	printf("Done waiting\n");
	//6. Stop candy-factory threads
	stop_thread = true;

	printf("Stopping candy-factories...\n");
	for(int i = 0; i < factNum; i++){
		//pthread_cancel(factThreads[i]);
		pthread_join(factThreads[i], NULL);
	}
	//printf("Factories stopped\n");

	//7. Wait until no more candy
	while(!bbuff_is_empty()){
		printf("Waiting for all candy to be consumed\n");
		sleep(1);
	}

	//8. Stop kid threads
	for(int i = 0; i < kidNum; i++){
		pthread_cancel(kidThreads[i]);
		pthread_join(kidThreads[i], NULL);
		//printf("%d...", i);
	}
	//printf("Kids stopped\n");

	//9. Print statistics
	stats_display();

	//10. Cleanup any allocated memory
	stats_cleanup();

	return 0;
}
#include "bbuff.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>

void* buffer[BUFFER_SIZE];
sem_t bufferFull;
sem_t bufferEmpty;
sem_t mutex;
int index;

void bbuff_init(void){
	sem_init(&bufferFull, 0, 0);
	sem_init(&bufferEmpty, 0, BUFFER_SIZE);
	sem_init(&mutex, 0, 1);
	index = 0;
	//printf("buffer initiated");
}

void bbuff_blocking_insert(void* item){
	sem_wait(&bufferEmpty);
	sem_wait(&mutex);

	buffer[index] = item;
	index++; //1 over
	//printf("inserting...\n");

	sem_post(&mutex);
	sem_post(&bufferFull);
}

void* bbuff_blocking_extract(void){
	sem_wait(&bufferFull);
	sem_wait(&mutex);

	index--; //current
	void* candy = buffer[index];
	buffer[index] = NULL;

	sem_post(&mutex);
	sem_post(&bufferEmpty);

	return candy;
}
_Bool bbuff_is_empty(void){
	return index == 0;
}
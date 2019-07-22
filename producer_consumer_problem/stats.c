#include "stats.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

typedef struct{
	int fact;
	int made;
	int eaten;
	double delayMin;
	double delayAvg;
	double delayMax;
	double delayTotal;
} stats_t;

stats_t* stats;
int factCount = 0;

void stats_init(int num_producers){
	stats = malloc(num_producers * sizeof(stats_t));

	for(int i = 0; i < num_producers; i++){
		stats[i].fact = i;
		stats[i].made = 0;
		stats[i].eaten = 0;
		stats[i].delayMin = INT_MAX;
		stats[i].delayAvg = 0;
		stats[i].delayMax = -1;
		stats[i].delayTotal = 0;
	}

	factCount = num_producers;
}
void stats_cleanup(void){
	free(stats);
}

void stats_record_produced(int factory_number){
	stats[factory_number].made++;
}

void stats_record_consumed(int factory_number, double delay_in_ms){
	stats[factory_number].eaten++;

	if(stats[factory_number].delayMin > delay_in_ms) stats[factory_number].delayMin = delay_in_ms;
	if(stats[factory_number].delayMax < delay_in_ms) stats[factory_number].delayMax = delay_in_ms;
	
	stats[factory_number].delayTotal += delay_in_ms;
	stats[factory_number].delayAvg = stats[factory_number].delayTotal / stats[factory_number].made;
}

void stats_display(void){
	printf("%s%10s%10s%15s%15s%15s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");

	for(int i = 0; i < factCount; i++){
		if(stats[i].made != stats[i].eaten) printf("ERROR: Mismatch between number made and eaten.\n");
		printf("%8d%10d%10d%15.5f%15.5f%15.5f\n", stats[i].fact, stats[i].made, stats[i].eaten, stats[i].delayMin, stats[i].delayAvg, stats[i].delayMax);
	}
}

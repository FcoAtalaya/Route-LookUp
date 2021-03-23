#include "my_route_lookup.h"
#include "io.h"




#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>





double totalTableAccesses;
int processedPackets;
double totalPacketProcessingTime;

int error;
struct routing_table *rt;
struct timespec* start, *end;

int main (int argc, char *argv[]){
	
	start=(struct timespec*)malloc(sizeof(struct timespec));
	end=(struct timespec*)malloc(sizeof(struct timespec));
	
	totalTableAccesses = 0;
	processedPackets = 0;
	totalPacketProcessingTime = 0;


	if(argc!=3){
		printf("Error not enough argument\n");
		return -1; //pa tu casa
	}
	
	
	
	error=initializeIO(argv[1], argv[2]);		//initialize intput files
	if(error!=0){							
		printIOExplanationError(error);
		return -1;
	}
	
	rt=initialize_rt(rt);					//initialize routing tables
	
	if(rt==NULL){
		printf("Error initializing tables");
		return -1;
	}
	
		
	rt=fill_rt(rt);					//initialize routing tables
	if(rt==NULL){
		printf("Error initializing tables");
		return -1;
	}
	
	error=routing(rt, end, start, &totalTableAccesses, &processedPackets, &totalPacketProcessingTime);		//looking for IP prefix
	if(error!=0){							
		printf("Error routing");
		return -1;
	}
	
	double averageTableAccesses = totalTableAccesses / processedPackets;
	double averagePacketProcessingTime = totalPacketProcessingTime / processedPackets;
	printSummary(processedPackets, averageTableAccesses, averagePacketProcessingTime);
	
	
	free_rt(rt);
	freeIO();
	free(start);free(end);
	
	return 0;
	
	
}

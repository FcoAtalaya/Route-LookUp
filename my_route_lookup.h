#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define table24_length pow(2,24)
#define table8_length pow(2,8)

struct routing_table{
	short *table24;
	short *table8;
	short index;		// position of the entrie of the first table
	int addedTable;	//number of 2^8 entries added to second table
};



struct routing_table* initialize_rt(struct routing_table * rt);

struct routing_table* fill_rt(struct routing_table* rt);	

struct routing_table * add_route_to_table (struct routing_table *rt, uint32_t ip_prefix, int mask, int interface);

int lookup (struct routing_table* rt, uint32_t *ip, int *which_table, int *interface);

int routing (struct routing_table* rt, struct timespec* end, struct timespec* start, double *totalTableAccesses, int *processedPackets, double* totalPacketProcessingTime);

void free_rt(struct routing_table * rt);

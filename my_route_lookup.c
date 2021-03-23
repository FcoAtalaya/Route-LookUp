#include "my_route_lookup.h"
#include "io.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


struct routing_table* initialize_rt(struct routing_table* rt){
	
	rt= ( struct routing_table*) malloc(sizeof(struct routing_table));
	
	rt->table24=(short *)calloc(table24_length, sizeof(short)); 
	
	rt->table8=NULL;
	rt->index=0;
	rt->addedTable=0;
	
	return rt;
}

struct routing_table * add_route_to_table (struct routing_table *rt, uint32_t ip_prefix, int mask, int interface){
	
	short outputs=0;
	
	if(mask<=24){
						//case: first table search
		outputs=pow(2,24-mask);
		
		for(rt->index =0 ; rt->index < outputs; rt->index++){
			
			rt->table24[(ip_prefix>>8) + rt->index ]=interface;	//writes as interfaces as outputs exists
			
		}
		
	}else{							//case: second table search
		outputs= pow(2, 32-mask);
		
		if( rt->table24[ip_prefix>>8]>>15 ==0){	//if first bit of short entrie is 0: no second table
			
			if(rt->table8==NULL){
				
				rt->table8=(short*)calloc(256 *2, sizeof(short));
				
			}else{
			
				rt->table8=realloc(rt->table8, 256*(rt->addedTable +1) *2);	//add 2^8 empty entries
			
			}
			
			for(rt->index=0; rt->index<=255; rt->index++){
				
				rt->table8[rt->addedTable*256 + rt->index]= rt->table24[ip_prefix>>8];	//fill table8 with table24 prefixes
			}
			
			
			rt->table24[ip_prefix>>8]=rt->addedTable - pow(2,15);	//removes the first bit of the first table
			
			
			for(rt->index=(ip_prefix & 0xFF); rt->index<(outputs + (ip_prefix & 0xFF)); rt->index++){
				
				rt->table8[ rt->addedTable*256 + rt->index] = interface;
			}
			rt->addedTable++;
			
		}else{									//if second table exists
			
			for(rt->index=(ip_prefix & 0xFF); rt->index<(outputs + (ip_prefix & 0xFF)); rt->index++){
				
				rt->table8[ (rt->table24[ip_prefix>>8] & 0x7FFF)*256  +  rt->index ] = interface;
			}
			
		}
	
	
	}
	
	return rt;
	
}


struct routing_table* fill_rt (struct routing_table * rt){
	
	int error=0;
	uint32_t ip_prefix=0;
	int mask=0;
	int interface=0;
	
	error=readFIBLine(&ip_prefix,&mask,&interface);	//read the line of the ip prefix tables
	
	while(error==0){
		rt=add_route_to_table(rt,ip_prefix,mask,interface);	//add to the routing table if no error
		
		error=readFIBLine(&ip_prefix,&mask,&interface);	
			
	}
	
	return rt;
}

int lookup (struct routing_table* rt, uint32_t *ip, int *which_table, int* interface){
	long position;
	
	*interface=rt->table24[*ip>>8];
	
	if( (*interface>>15)==0 ){
		*which_table=1;			//search in table24
	}else{
		*which_table=2;			//searchs in table8
		
		position = (*interface & 0x7FFF) *256  + (*ip & 0x000000FF);
		
		*interface=rt->table8[ position ];
	}
	
	
	
	return 0;
	
	
}


int routing(struct routing_table* rt, struct timespec* end, struct timespec* start, double *totalTableAccesses, int *processedPackets, double* totalPacketProcessingTime){
	
	uint32_t  ip=0;
	int interface=0;
	double searching_time =0;
	int which_table =0;
	
	
	
	int error=0;
	
	error= readInputPacketFileLine(&ip);		//takes the ip addr of the file
	while(error==0){
		
		clock_gettime(CLOCK_MONOTONIC_RAW, start);		//starts counting
		
		lookup(rt, &ip, &which_table, &interface);		//searchs ip addr in the routing tables
		
		clock_gettime(CLOCK_MONOTONIC_RAW, end);		//stops counting
		
		printOutputLine(ip, interface, start, end, &searching_time, which_table);	//writes in the output file
		
		*processedPackets = *processedPackets + 1;
		*totalTableAccesses  = *totalTableAccesses + which_table;
		*totalPacketProcessingTime  = *totalPacketProcessingTime + searching_time;
		
		error = readInputPacketFileLine(&ip);
		
	}
	
return 0;
	
}



void free_rt(struct routing_table* rt){
	
	free(rt->table24);
	free(rt->table8);
	free(rt);
	
}



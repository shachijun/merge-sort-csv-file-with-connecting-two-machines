#include <stdio.h>
#include <stdlib.h>
#include "readbuf.h"
#include <pthread.h>
pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER; 

void enlarge(struct bufarg **bufarray)
{
	int size = (*bufarray[0]).id_size;
	*bufarray = realloc(bufarray, size*2);
	(*bufarray)[0].id_size = size*2;
	int i;
	for(i = size; i < size*2; i++){
		(*bufarray)[i].sess_id = i;
		(*bufarray)[i].isFree = 1;
	}
}

struct bufarg *init_array()
{
	struct bufarg *bufarray = malloc(10 * sizeof(struct bufarg));
	bufarray[0].id_size = 10;
	int i;
	for(i = 0; i < 10; i++){
		bufarray[i].sess_id = i;
		bufarray[i].isFree = 1;
	}
	return bufarray;
	
}

//return an available session id
int get_id(char *field_name, struct bufarg **bufarray)
{

	pthread_mutex_lock(&locker);
	int size =(*bufarray)[0].id_size;
	int i;
	for(i = 0; i < size; i++){
		if((*bufarray)[i].isFree == 1){
			(*bufarray)[i].isFree = 0;
			(*bufarray)[i].table = initialize_csv();
			(*bufarray)[i].field_num = get_field_index(field_name);
			return i;
		}
	}
	enlarge(bufarray);
	(*bufarray)[i].isFree = 0;
	(*bufarray)[i].table = initialize_csv();
	(*bufarray)[i].field_num = get_field_index(field_name);
	pthread_mutex_unlock(&locker);
	return i;
}

void free_id(struct bufarg *bufarray, int i)
{
	struct csv *table;
	table = bufarray[i].table;
	bufarray[i].table = NULL;
	free_csv(table);
	bufarray[i].isFree = 1;
}
	
	
		

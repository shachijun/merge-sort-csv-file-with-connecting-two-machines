#ifndef _BUF_STORE_H
#define _BUF_STORE_H

struct sarg{
	struct bufarg *id_list;
	int socketfd;
};	

struct bufarg{
	struct csv *table;
	int sess_id;
	int id_size;
	int isFree;
	int field_num;
};

#include "readbuf.h"
void enlarge(struct bufarg**);
struct bufarg* init_array();
int get_id(char *, struct bufarg**);
void free_id(struct bufarg*, int);
#endif 

#ifndef _MERGESORT_H
#define _MERGESORT_H

//pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER;

char ***smatrix;
 
struct mergesort_args {
	char ***matrix;
	//struct tid_list *tids;
	int field_index;
	int low;
	int high;
};

void mergesort(int low, int high, int field, char ***matrix);

#endif

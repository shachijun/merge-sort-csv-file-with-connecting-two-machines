#ifndef _CSV_H
#define _CSV_H
#include "buf_store.h"
struct csv {
	char ***matrix;
	pthread_mutex_t mutex;
	int num_rows;
	int row_capacity;
	int t_length;
};

struct csv* initialize_csv();
void readbuf(char *buffer, struct csv *table, int len);
void append_file(char *file, int len, int sid, struct bufarg* ba);
void append_csv(struct csv *table, char ***new_entries, int num_new, int length);
char *print_csv(struct bufarg args);
void free_csv(struct csv *table);
int get_field_index(char *);
#endif

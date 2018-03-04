#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "readbuf.h"
#include "mergesort.h"
#define NUM_COLS 28

char *header = "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes";

char *field_list[NUM_COLS] = {
	"color",
	"director_name",
	"num_critic_for_reviews",
	"duration",
	"director_facebook_likes",
	"actor_3_facebook_likes",
	"actor_2_name",
	"actor_1_facebook_likes",
	"gross",
	"genres",
	"actor_1_name",
	"movie_title",
	"num_voted_users",
	"cast_total_facebook_likes",
	"actor_3_name",
	"facenumber_in_poster",
	"plot_keywords",
	"movie_imdb_link",
	"num_user_for_reviews",
	"language",
	"country",
	"content_rating",
	"budget",
	"title_year",
	"actor_2_facebook_likes",
	"imdb_score",
	"aspect_ratio",
	"movie_facebook_likes",
};

struct csv *initialize_csv()
{
	struct csv *table = malloc(sizeof(struct csv));
	table->matrix = NULL;
	pthread_mutex_init(&table->mutex, NULL);
	table->num_rows = 0;
	table->row_capacity = 0;
	table -> t_length = 0;
	return table;
}


//csvread would take a buffer,a matrix as an argument
void readbuf(char *buffer, struct csv *table, int len)
{
	
	char ***matrix;
	int row;
	int row_capacity;
	char *line, *linep;
	char *str;
	char *strex;
	int i;
	//printf("first line is %.*s\n", 10, buffer);
	line = strtok(buffer, "\r\n");
	//printf("header %s\n", line);
	if (line == NULL || strcmp(line, header)) {
		printf("header length %d, line length is %d\n", strlen(header), strlen(line));
		fputs("invalid header\n", stderr);
		free(buffer);
		return;
	}
	row_capacity = 64;
	matrix = malloc(row_capacity * sizeof(*matrix));
	row = 0;
	while ((line = strtok(NULL, "\r\n")) != NULL) {
		if (row >= row_capacity) {
			row_capacity *= 2;
			matrix = realloc(matrix, row_capacity * sizeof(*matrix));
		}
		matrix[row] = malloc(NUM_COLS * sizeof(**matrix));
		linep = line;
		for (i = 0, str = strsep(&linep, ","); str; i++, str = strsep(&linep, ",")) {
			if (strlen(str) == 0) {
				matrix[row][i] = NULL;
			} else if (str[0] == '"') {
				strex = strsep(&linep, "\"");
				matrix[row][i] = malloc((strlen(str) + strlen(strex) + 1) * sizeof(char));
				sprintf(matrix[row][i], "%s,%s", str + 1, strex);
				strsep(&linep, ",");
			} else {
				matrix[row][i] = malloc((strlen(str) + 1) * sizeof(char));
				strcpy(matrix[row][i], str);
			}
		}
		row++;
	}
	free(buffer);
	if(row == 0)
		return;
	if(table == NULL)	
		table = calloc(1, sizeof(struct csv));
	append_csv(table, matrix, row, len);
	free(matrix);
	return;
}



void append_file(char *file, int len, int sid, struct bufarg *ba)
{
	readbuf(file, ba[sid].table, len);
}

void append_csv(struct csv *table, char ***new_entries, int num_new, int len)
{
	pthread_mutex_lock(&table->mutex);
	table ->t_length += len;
	if (table->num_rows + num_new > table->row_capacity) {
		table->row_capacity = 2 * (table->num_rows + num_new);
		table->matrix = realloc(table->matrix, table->row_capacity * sizeof(*table->matrix));
	}
	memcpy(table->matrix + table->num_rows, new_entries, num_new * sizeof(*table->matrix));
	table->num_rows += num_new;
	pthread_mutex_unlock(&table->mutex);
}

char *print_csv(struct bufarg buf)
{

	int i, j;
	//printf("mergesort arguments 1: 0, %d\n", buf.field_num);
	//printf("mergesort arguments 2: %d\n", buf.table -> num_rows);
	if(buf.table == NULL)
		return NULL;
	smatrix = malloc(sizeof(char **) * (buf.table -> num_rows));	
	mergesort(0, (buf).table -> num_rows, buf.field_num, (buf).table -> matrix);
	struct csv *table = buf.table;
	
	int length = buf.table -> t_length;
	printf("length after sorting is %d\n", length);
	char *buffer = malloc(length+1);
	char *ptr;
	sprintf(buffer, "%s\r\n", header);
	ptr = buffer + strlen(buffer);
	for (i = 0; i < table->num_rows; i++) {
		for (j = 0; j < NUM_COLS; j++) {
			if (smatrix[i][j]) {
				if (index(smatrix[i][j], ',')){
					//printf("smatrix[%d][%d] is %s\n", i, j, smatrix[i][j]);
					sprintf(ptr, "\"%s\"", smatrix[i][j]);
					ptr += strlen(ptr);
				}
				else{
					sprintf(ptr, smatrix[i][j]);
					ptr += strlen(ptr);
				}
			}		
			if (j < NUM_COLS - 1){
				sprintf(ptr, ",");
				ptr += strlen(ptr);
			}
		}
		sprintf(ptr, "\r\n");
		ptr += strlen(ptr);
	}
	//printf("buffer is %s\n", buffer);
	return buffer;
	
}

void free_csv(struct csv *table)
{
	int i, j;
	for (i = 0; i < table->num_rows; i++) {
		for (j = 0; j < NUM_COLS; j++)
			free(table->matrix[i][j]);
		free(table->matrix[i]);
	}
	free(table->matrix);
	pthread_mutex_destroy(&table->mutex);
}



int get_field_index(char *field_name)
{
	int i;
	for (i = 0; i < NUM_COLS; i++) {
		if (strcmp(field_list[i], field_name) == 0) {
			return i;
		}
	}
	fprintf(stderr, "error: invalid field \"%s\"\n", field_name);
	return -1;
}

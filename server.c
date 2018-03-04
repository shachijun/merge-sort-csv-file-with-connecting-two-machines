#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "buf_store.h"
#include "readbuf.h"

const int BUF_LEN = 256;
const int HDR_LEN = 128;
const int PAD_LEN = 128;
const int BACKLOG = 16;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

struct service_args {
    int fd;
    struct bufarg **ba;
};

void *service(void *arg);

int main(int argc, char **argv)
{
    int c, port, sock, fd;
    struct sockaddr_in addr;
    struct bufarg *ba;
    struct service_args *sa;
    pthread_t tid;
    port = 0;
    fprintf(stdout, "Received connections from: ");
    while ((c = getopt(argc, argv, "p:")) != -1) {
        switch (c) {
        case 'p':
            port = atoi(optarg);
        }
    }
    if (port == 0) {
        fputs("please provide a port number\n", stderr);
        return 1;
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        fputs("failed to create socket\n", stderr);
        return 1;
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        fprintf(stderr, "failed to bind socket %d to port %d\n", sock, port);
        return 1;
    }
    if (listen(sock, BACKLOG) == -1) {
        fprintf(stderr, "failed to listen on socket %d with backlog %d\n", sock, BACKLOG);
        return 1;
    }
    ba = init_array();
    while (1) {
	printf("waiting for connections\n\n\n");
        if ((fd = accept(sock, NULL, NULL)) == -1) {
            fprintf(stderr, "failed to accept connection on socket %d\n", sock);
            return 1;
        }
        sa = (struct service_args *)malloc(sizeof(struct service_args));
        sa->fd = fd;
        sa->ba = &ba;
        if (pthread_create(&tid, NULL, &service, sa)) {
            fputs("failed to create thread\n", stderr);
            free(sa);
            return 1;
        }
    }
    return 0;
}

void *service(void *arg)
{
    struct service_args *args;
    struct bufarg **ba;
    int fd, sid, len, i;
    char buffer[BUF_LEN], field_name[BUF_LEN], *file, *fptr;
    args = (struct service_args *) arg;
    fd = args->fd;
    ba = args->ba;
    read(fd, buffer, HDR_LEN);
    if (strncmp(buffer, "QUIT_SERVER", 11) == 0) {
        if (sscanf(buffer, "QUIT_SERVER-_-%d", &sid) == 1 && sid < (*ba)->id_size && (*ba)[sid].isFree == 0) {
	    //printf("quit server \n");
	    //printf("session id is %d, num row is %d, word length is %d\n", sid,(*ba[sid]).table -> num_rows, (*ba[sid]).table -> t_length);
            file = print_csv(*ba[sid]);
	    if (file == NULL){
		return NULL;
	    }
            len = strlen(file);
	    //printf("file is %s\n", file);
	    //printf("file length is %d\n", len);
            sprintf(buffer, "%ld", strlen(file));
            write(fd, buffer, HDR_LEN);


	    //write(fd, file, strlen(file));


int size=0;
int len=strlen(file);
        //By Chijun Sha end
do{
        size+=write(fd, file+size, len-size);
}while(size<len);
// printf("\n\n\n\ndone hrererhg\n\n\n\n\n");



char rbuffer[256];
   int n = read(fd, rbuffer, sizeof(char)*4);
       	//printf("rbuffer is %s\n", rbuffer); 
        if(n < 0){
            perror("read");
        }
	// printf("\n\n\n\nfsadfdasfdfsdadone hrererhg%s\n\n\n\n\n",rbuffer);     
/*
            for (fptr = file, i = 0; i < len - BUF_LEN; fptr += a, i += a) {
                strncpy(buffer, fptr, BUF_LEN);
		printf("current buffer is %s\n", buffer);	
                a =  write(fd, buffer, BUF_LEN);
            }

*/	    
            free(file);
        }
    } else if (strncmp(buffer, "Get_Id", 6) == 0) {
        if (sscanf(buffer, "Get_Id-_-%s", field_name) == 1) {   
	    sid = get_id(field_name, ba);
            sprintf(buffer, "1%d", sid);
	    printf("can get id\n");
            write(fd, buffer, HDR_LEN);
        }
    } else if (sscanf(buffer, "%d-_-%d", &sid, &len) == 2) {
	printf("read data\n");
        read(fd, buffer, PAD_LEN);
        file = malloc((len + 1) * sizeof(char));
	int a = 0;

        for (fptr = file, i = 0; i <= len - BUF_LEN; fptr += a, i += a) {
            a = read(fd, buffer, BUF_LEN);
            strncpy(fptr, buffer, BUF_LEN);
        }
	printf("len is %d\n", len);
        read(fd, buffer, len - i);
        strncpy(fptr, buffer, len - i);
        file[len - 1] = '\0';
	//printf("\n\nfile is %s\n",file);
        
        pthread_mutex_lock(&lock);
        append_file(file, len, sid, *ba);
        pthread_mutex_unlock(&lock);
	a = write(fd, "done", 4);
	if ( a < 0){
		perror("write");
	}	
	//printf("row num is %d\n", *ba[sid] -> table -> num_rows);
    }
    close(fd);
    free(arg);
    return NULL;
}

#ifndef sorter_h
#define sorter_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*directory Stuff*/
#include <unistd.h>
#include <dirent.h>
#include <getopt.h>
#include <malloc.h>
/* Socket Stuff */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
/* others */
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
//#include "readbuf.c"
//typedef struct Node{
//    char** col;
//    int Golcolomn;
//    int Golrow;
//    int typelocate;//this is index
//    int signal;//0 means return 1 means sort this csv
//}Node;
typedef struct LinkList{
    pthread_t data;
    struct LinkList* next;
}LinkList;
#endif /* sorter_h */

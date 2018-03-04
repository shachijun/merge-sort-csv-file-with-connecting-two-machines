#include "sorter_client.h"

char *sortedBy = NULL;
char * inppath=NULL;
char *output = NULL;
/*for socket*/
int port=-1;
struct sockaddr_in address;
struct hostent *server;
/*Extra credit*/
int pool=-1;
sem_t sem_name;
char id[256];
LinkList* pidCounter=NULL;
const int BUF_LEN = 256;

void CallServer(FILE *fptr,char*node){// with finish signal
    int sockfd;
    if (pool>0) {
        sem_wait(&sem_name);
    }
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("sock");
        exit(EXIT_FAILURE);
    }
    if (connect(sockfd, (struct sockaddr*)&address, sizeof(address))<0){
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if(strcmp(node,"QUIT_SERVER")==0){
        //        here is write, tell server to stop
	char send[128];
        sprintf(send, "QUIT_SERVER-_-%s", id);

        int n = write(sockfd, send, sizeof(char)*128);
        if(n <= 0){
            perror("write");
        }
        char rebeg[128];
        n = read(sockfd, rebeg, sizeof(char)*128);
        if(n <= 0){
            perror("read");
        }
	//printf("rebeg is %s\n", rebeg);
        int len=atoi(rebeg);
	//printf("receive size is %d\n", size);
 char buffer[BUF_LEN];
        FILE *fe;
        //remove("output.csv");
        int length=strlen(output)+24+strlen(sortedBy);
        char fileN[length];
        fileN[length-1]='\0';
        strcpy(fileN,output);
        strcat(fileN,"/");
        strcat(fileN,"AllFiles-sorted-<");
        strcat(fileN,sortedBy);
        strcat(fileN,">.csv");
        fe = fopen(fileN,"w+");
        int a = 0;
char * file,*fptr;
int i=0;
        file = malloc((len + 1) * sizeof(char));
        for (fptr = file, i = 0; i <= len - BUF_LEN; fptr += a, i += a) {
            a = read(sockfd, buffer, BUF_LEN);
printf("line %s\n\n\n",buffer);
if(a<0){
perror("read final");
break;
}
            strncpy(fptr, buffer, BUF_LEN);
            memset(buffer, 0,  BUF_LEN);
        }
	printf("len is %d\n", len);
        read(sockfd, buffer, len - i);
        strncpy(fptr, buffer, len - i);
file[len]='\0';

        fwrite(file, 1, len, fe);
a = write(sockfd, "done", 4);
	if ( a < 0){
		perror("write");
	}
        fclose(fe);
	close(sockfd);
    }else if(strcmp(node,"Get_Id")==0){
        printf("get in get id\n\n\n\n\n\n");
	char send[256];
        sprintf(send, "Get_Id-_-%s", sortedBy);
        int n = write(sockfd, send , sizeof(char)*256);
        if(n < 0){
            perror("write");
        }
        char recv[256];
        n = read(sockfd, recv, sizeof(char)*256);
        if(n < 0){
            perror("read");
        }
        if (strncmp(recv,"0",1)==0) {
            exit(1);
        }else{
            strcpy(id,recv+1);
        }
        printf("now the id if|%s|",id);
    }else{
        fseek(fptr, 0, SEEK_END);
        int len = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);
        char *buffer = malloc(len*sizeof(char));
        fread(buffer, len, 1, fptr);
        char send[265];
        sprintf(send,"%s-_-%d",id,len);
        int n = write(sockfd,send , sizeof(char)*256);
        if(n <= 0){
            perror("write");
        }
        /* this is replacing
         char*strret=NULL;
         do{
         strret=strstr(buffer,"\r");
         if(strret==NULL){
         break;
         }
         int pos=strret-buffer;
         buffer[pos]='^';
         buffer[pos+1]='_';
         }while(1);
         */
        n = write(sockfd, buffer, sizeof(char)*len);
        if (n <= 0){
            perror("write");
        }
        free(buffer);
        char rbuffer[256];
        n = read(sockfd, rbuffer, sizeof(char)*4);
       	//printf("rbuffer is %s\n", rbuffer); 
        if(n < 0){
            perror("read");
        }
        //printf("from server: %s\n", rbuffer);
        //while (strcmp(rbuffer, "done") == 0);
    }
    if (pool>0) {
        sem_post(&sem_name);
    }
}



void*FindCsv(void* dirtory){
    FILE*fp=fopen((char*)dirtory,"r");
    if( fp== NULL){
        pthread_exit(NULL);
    } else{
        CallServer(fp,"sort");
        pthread_exit(NULL);
    }
}



void* FindDir(char * dirtory){
    DIR * dir;
    struct dirent *st;
    dir=opendir(dirtory);
    char path[1024];
    if (NULL == dir){
        fprintf(stderr, "Error : Failed to open input directory\n");
        exit(1);}
    while ( (st = readdir(dir)) != NULL) {
        if ( *(st->d_name) == '.'){
            continue;
        }
        strcpy(path ,dirtory);
        strcat(path , "/");
        strcat(path , st->d_name);
        if (st->d_type == DT_DIR) {
            char * getph=(char *) malloc((strlen(path)+1)*sizeof(char));
            getph[strlen(path)]='\0';
            strcpy(getph, path);
            FindDir(getph);
        }
        //csv file
        if ( strcmp((st->d_name+(strlen(st->d_name)-4)), ".csv")==0 ) {
            char * getph=(char *) malloc((strlen(path)+1)*sizeof(char));
            getph[strlen(path)]='\0';
            strcpy(getph, path);
            pthread_t pid;
            //int err = 0;
            //FindCsv(getph);
            int err = pthread_create(&pid, NULL, FindCsv,(void*)getph);
            if (err != 0){
                printf("\n can't create thread: [%s]\n", strerror(err));}
            if(pidCounter==NULL){
                pidCounter=( LinkList*)malloc(sizeof(LinkList));
                pidCounter->data=pid;
                pidCounter->next=NULL;
            }else{
                LinkList* pidCounter1=( LinkList*)malloc(sizeof( LinkList));
                pidCounter1->data=pid;
                pidCounter1->next=pidCounter;
                pidCounter=pidCounter1;
            }
        }
    }
	return NULL;
}



int main(int argc, char * const argv[]) {
    char input_path[1024];
    int c=0;
    char* output_path=NULL;
    getcwd( input_path, 1024);
    
    while((c = getopt (argc, argv, "c: d: o: p: h: s:")) != -1){
        switch (c)
        {   case 'c':
                sortedBy = optarg;
                break;
            case 'd':
                if (optarg == NULL){
                    fprintf(stderr, "no start path\n");
                    exit(1);
                }
                strcpy(input_path, optarg);
                break;
            case 'o':
                if (optarg == NULL){
                    fprintf(stderr, "no output path\n");
                    exit(1);
                }
                output_path = optarg;
                break;
            case 'p'://has to be integer?
                if (optarg == NULL){
                    fprintf(stderr, "no output path\n");
                    exit(1);
                }
                port = atoi(optarg);
                break;
            case 's':
                if (optarg == NULL){
                    fprintf(stderr, "no output path\n");
                    exit(1);
                }
                pool = atoi(optarg);
                break;
            case 'h':
                if (optarg == NULL){
                    fprintf(stderr, "no output path\n");
                    exit(1);
                }
                server=gethostbyname(optarg);
                if (server == NULL)
                {
                    fprintf(stderr, "ERROR, NO SUCH HOST\n");
                    exit(2);
                }
                break;
            case '?':
                fprintf(stderr, "Unknown option\n");
                exit(1);
                break;
        }
    }
    //add sorted-by later
    if(input_path == NULL){
        perror("path error");
        exit(2);
    }
    if (port<0) {
        perror("port error");
        exit(2);
    }
    if (pool!=-1 && pool<=0) {
        perror("pool error");
        exit(2);
    }
    if (pool>0) {
        sem_init(&sem_name, 0, 10);
    }
    if(output_path==NULL) {
        output=(char*)malloc((strlen(input_path)+1)*sizeof(char));
        strcpy(output,input_path);
        output[strlen(input_path)]='\0';
    }else {
        output=(char*)malloc((strlen(output_path)+1)*sizeof(char));
        strcpy(output,output_path);
        output[strlen(output_path)]='\0';
    }
    inppath=input_path;
    memset(&address, 0, sizeof(address));
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    bcopy((char*)server->h_addr,
          (char*)&address.sin_addr.s_addr,
          server->h_length);
    //strat
    CallServer(NULL,"Get_Id");
    FindDir(input_path);
    while(pidCounter!=NULL){
        (void)pthread_join(pidCounter->data,NULL);
        LinkList*pidCount=pidCounter->next;
        pidCounter=pidCount;
    }
    //printf("quit\n");
    CallServer(NULL,"QUIT_SERVER");
    //end
    return 0;
    
}


CC = gcc
sources = buf_store.c mergesort.c readbuf.c server.c 
headers = buf_store.h mergesort.h readbuf.h 

.PHONY: all
all: server client

server: $(sources) $(headers)
	gcc -pthread -g -lm -Wall -o server $(sources)

client: sorter_client.c sorter_client.h
	gcc -pthread -g -lm -Wall -o client sorter_client.c

.PHONY: clean
clean:
	rm -vf client server 

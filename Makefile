all: main.c word_count.c buffered_queue.c hashmap.c
	gcc -std=gnu99 -g -o main -lpthread main.c word_count.c buffered_queue.c hashmap.c 


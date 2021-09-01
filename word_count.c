#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "word_count.h"
#include "hashmap.h"
#include "buffered_queue.h"
#define _GNU_SOURCE
struct buffered_queue *queue;

int print_result(char*, int);

struct mapper_arg{
    int r;
    char *text;
};

void *mapper(void *args){
    struct mapper_arg *arg = (struct mapper_arg*) args;
    char *file = arg->text; //change it to file
    const char delimiter[2] = " ";
    char *str= malloc(1000*sizeof(char));
//Open file and if null return error
    FILE* fp = fopen(file,"r");

    if(fp==NULL){
	fprintf(stderr,"Unable to read %s \n",file);
	return NULL;
    }
//Get line by line and remove \n from the strings 
    while(fgets(str,1000,fp)){
	for (char *p = str; (p = strchr(p, '\n')) != NULL; p++) {
    	    *p = ' ';
	}
    	char *saveptr = NULL;
    	char *word = strtok_r(str, delimiter, &saveptr);
    	while(word!=NULL){
		char *tmp = malloc(sizeof(char)*strlen(word)+1);
		strcpy(tmp, word);
		unsigned long crc = crc32((unsigned char *)word, strlen(word));	
		buffered_queue_push(&queue[crc%arg->r], tmp);
		word = strtok_r(NULL, delimiter, &saveptr);
    	}
    }

    for(int i=0; i<arg->r; i++){
	char *special = malloc(sizeof(char)*2);
	strcpy(special, "0");
	buffered_queue_push(&queue[i], special);
    }

   free(arg->text);
   free(arg);
   fclose(fp);
   free(str); 
   return NULL;
}

struct reducer_arg{
    int index;
    int m;
    int r;
};

void *reducer(void *args){
    struct reducer_arg *arg = (struct reducer_arg*)args;
    map_t mymap;
    mymap = hashmap_new();
    int finishedMappers = 0;
    while(1){
	char *word = buffered_queue_pop(&queue[arg->index]);
	if(strcmp(word, "0") == 0){
	    free(word);
	    finishedMappers ++;
	    if(finishedMappers == arg->m)
		break;	
	}
	else{
	    int count;
	    int error = hashmap_get(mymap, word, &count);
            if(error==MAP_OK)
                hashmap_put(mymap, word, count+1);
            else
                hashmap_put(mymap, word, 1);
	}	
    }
    hashmap_iterate(mymap, print_result);
    free(arg);
    hashmap_free(mymap);
    return NULL;
}

void word_count(int m, int r, char **text){
    queue = malloc(sizeof(struct buffered_queue)*r);
    pthread_t *mapper_id = malloc(sizeof(pthread_t)*m);
    pthread_t *reducer_id = malloc(sizeof(pthread_t)*r);

    for(int i=0; i<r; i++)
        buffered_queue_init(&queue[i], 1);

    for(int i=0; i<m; i++){
	struct mapper_arg *arg = malloc(sizeof(struct mapper_arg));
	arg->r = r;
	arg->text = text[i];//malloc(strlen(text[i])+1);
	//strcpy(arg->text, text[i]);
	pthread_create(&mapper_id[i], NULL, mapper, arg);
    }
	
    for(int i=0; i<r; i++){
        struct reducer_arg *arg = malloc(sizeof(struct reducer_arg));
	arg->m = m;
        arg->r = r;
	arg->index = i;
        pthread_create(&reducer_id[i], NULL, reducer, arg);
    }
    for(int i=0; i<m; i++)
	pthread_join(mapper_id[i], NULL);

    for(int i=0; i<r; i++)
	pthread_join(reducer_id[i], NULL);
    free(mapper_id);
    free(reducer_id);	
    for(int i=0; i<r; i++)
	buffered_queue_destroy(&queue[i]);
    free(queue);
}

int print_result(char *key, int value){
    int num = pthread_self();

    //Print to console
    printf("Count of %s = %d\n", key, value);
    FILE *fp;
 //Print to output files according to the pthread id
    char str[10000];
    char nums[100];
//Decreasing the number a bit to have shorter file names
    sprintf(nums,"%d",num/1000000);
    strcpy(str,"output");
    strcat(str,nums);
//Open file to start righting
    fp = fopen(str,"a");
    char buffer[100];
    sprintf(buffer,"%d",value);
//Get a string fpr the count of keys and values
    char str2[10000];
    strcpy(str2,"Count of ");
    strcat(str2,key);
    strcat(str2,"= ");
    strcat(str2,buffer);
    strcat(str2,"\n");
//Type the string in file
    fputs(str2,fp);
    fclose(fp);
    return 0;
}

  


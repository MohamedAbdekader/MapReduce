#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "word_count.h"

int main(int argc, char **args) {
    if(argc<=3){
	printf("Usage: ./main [m] [r] [path]\n");
	return -1;
    }
    int i = 0;
    int m = atoi(args[1]);
    int r = atoi(args[2]);
    char **files = malloc(sizeof(char*) * m);


/*Find file names in path.*/
    for(i = 0;i<m;i++){
	char *name = malloc(256);
	strcpy(name,args[3]);
	char input[7] ="/input";
	char num[2];
	sprintf(num,"%i",i);
	strcat(name,input);
	strcat(name,num);
	files[i]=name;
     } 

    printf("m=%d, r=%d \n", m, r);
    
    for(i=0; i<m; i++){
	printf("%s \n",files[i]);
    }
    printf("\n");
//Find count for files
    word_count(m, r, files);

    free(files);
    return 0;
}



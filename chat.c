#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chat.h"
//Function Prototypes
void fatal(char *); //func for fatal errors
void *ec_malloc(unsigned int);  //error checked malloc() wrapper
void *ec_realloc(char *str, unsigned int);
char *rmtrails(char *buf);

char *input(){

  size_t size = 1;

  char *str = ec_malloc (size * sizeof(char));

  char c;
  int i = 0;
  
  while(1){
    c = getc(stdin);
    
    //realloc memory by +1 each time, we use less mem this way
    //hopefully this will cause a heapoverflow inb4 instead of a bufferoverflow

    if(i >= size){
      size++;
      str = ec_realloc(str, size*sizeof(char)); 
    }
    if(c == '\n' || c == EOF) {
      str[i] == '\0';
      break;
    }else{
       str[i] = c;
    }
  i++;

  }

  return str;
}


char *addchar(const char *msg, const char *b){
  size_t len = strlen(msg);
  char *ret = ec_malloc(len * sizeof(char));
  *ret = '\0';
  return strcat(strcat(ret,msg), b);
}



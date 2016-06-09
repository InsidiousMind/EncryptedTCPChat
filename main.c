#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "chat.h"
#include "server.h"

int main(int argc, char *argv[]){
 char *msg; 
  
 init_server();

 while(1){
    printf("%s", "msg: ");
    msg = input();
    if(strcmp(msg,"/exit") == 0){
      killinc();
      exit(0);
    }else{
     msg = newline(msg);  
     sendmesg(msg); 
     bzero(msg,strlen(msg));
    }

  free(msg); 
  }
 
 killinc();
  

}

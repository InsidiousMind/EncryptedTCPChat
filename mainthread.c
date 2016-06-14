#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "chat.h"

char *username;
int retm = 0;


void setuser(char *buf){
  username = buf;
}

int mainthread(){
 
 char *msg; 
 
 
 init_server();

 while(1){
    printf("%s%s", username, ": ");
    msg = input();

    if(strlen(msg) == 0){
      setmsg("\n");
    }
    else if(strcmp(msg,"/exit") == 0){
      free(msg);
      killout();
      pthread_exit(&retm);
    }else{
      char *newline = "\n";
      msg = addchar(msg, newline);
      setmsg(msg);
      pthread_t tid;
      pthread_create(&tid, NULL, sendmesgptr, NULL);
      pthread_join(tid, NULL);
    }
    free(msg);

  }
 
 killout();
 killinc();
  
}
void *mainthreadptr(){
  mainthread();  
}

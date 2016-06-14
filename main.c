#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "chat.h"

int main(int argc, char *argv[]){
 
 
 
  if( argc < 2 || argc > 3){
    fprintf(stderr,"usage: 'TCPchat hostname [optional]username'\t\n");
    exit(1);
  }
  if(argc == 3){
    sethost(argv[1]);
    setuser(argv[2]);  
  }else{
    sethost(argv[1]);
    setuser("default");
  }

pthread_t tid1, tid2;

pthread_create(&tid1, NULL, mainthreadptr, NULL);
sleep(1);
pthread_create(&tid2, NULL, initclientptr, NULL);

printf("Closing Connection...\n");

sleep(5);

pthread_join(tid1,NULL);
pthread_join(tid2,NULL);


}




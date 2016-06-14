#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT "3490"

#define MAXDATASIZE 2048 //max # bytes we can receive at once

int ret = 0;
int ret1 = 1;
int ret2 = 2;
char *hostname;
int sockfd, numbytes;

void sethost(char *buf){
  hostname = buf;
}

void *get_in_addrc(struct sockaddr *sa){

  if(sa->sa_family == AF_INET){
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);

}

int initclient(char *hostname){
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if((rv=getaddrinfo(hostname, PORT, &hints, &servinfo))!=0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    pthread_exit(&ret1);
  } 
  
  
  while(1){ 
  for(p = servinfo; p!=NULL; p = p->ai_next){
    if((sockfd = socket(p->ai_family,p->ai_socktype,
            p->ai_protocol)) == -1){
      perror("[!!][client] socket");
      continue;
    }
    
    if(connect(sockfd, p->ai_addr,p->ai_addrlen) == -1){
      close(sockfd);
      perror("[!!][client] connect failed");
      continue;
    }
    
    break;
  
  } 

  if(p==NULL){
    fprintf(stderr,"[!!][client] failed to connet\n");
    pthread_exit(&ret2);
  }

    inet_ntop(p->ai_family, get_in_addrc((struct sockaddr*)p->ai_addr),
        s, sizeof(s));
    printf("[client] connecting to %s\n",s);
    
    freeaddrinfo(servinfo);

    break;
  }
    while(1){
    if((numbytes = recv(sockfd, buf, MAXDATASIZE-1,0)) == -1){
      perror("[!!][client] error receiving info");
      pthread_exit(&ret1);
    }

    buf[numbytes] = '\0';

    printf("[client] received '%s'\n",buf);

    }

  return 0;

}
void *initclientptr(){
  initclient(hostname);
  return NULL;
}

void killinc(){
  close(sockfd);
}
   





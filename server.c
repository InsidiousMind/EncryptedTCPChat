#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490" // le beautiful port

#define BACKLOG 10 // # of pending conn queue will hold

#define NUM_THREADS 2

int sockfd, new_fd; // listen on sockfd, new conn on new_fd (fd == file descriptor)
struct addrinfo hints, *servinfo, *p; 
struct sockaddr_storage their_addr; // remotehost addr info 
socklen_t sin_size;
struct sigaction sa;
int yes=1;
char s[INET6_ADDRSTRLEN];
int rv;


void sigchild_handler(int s){
  // waitpid() might overwrite errno, so we save and restore it:
  //
  int saved_errno = errno;

  while(waitpid(-1, NULL, WNOHANG) > 0); // wait for child process to stop/term

  errno = saved_errno;
}



void *get_in_addr(struct sockaddr *sa){
  if (sa -> sa_family == AF_INET){ //if IPv4
    return &(((struct sockaddr_in*)sa)->sin_addr);//return the addr of 'sin'
  }
  //else we have an IPv6 addr
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int init_server(){
  

  memset(&hints, 0, sizeof(hints)); //zero the struct
  hints.ai_family = AF_UNSPEC;    //set the type of connection to IPv4/6
  hints.ai_socktype = SOCK_STREAM;  //TCP stream sockets
  hints.ai_flags = AI_PASSIVE;    //fill in IP for me

  if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1; 
  }

  //loop through results and bind to first we can
  for(p = servinfo; p != NULL; p = p -> ai_next){
    if((sockfd = socket(p->ai_family, p->ai_socktype,   //get a socket desc
          p->ai_protocol)) == -1){
            perror("[!ERROR!!] server: socket");
           continue;
    }
  
    //just in case the socket we are trying to bind() to is already in use 
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
      perror("setsockopt");
      exit(1);
    }

    if(bind(sockfd, p->ai_addr,p->ai_addrlen) == -1){   //bind to it
      close(sockfd);
      perror("[!ERROR!] server bind failed");
      continue;
    }

    break;
  
  }

  freeaddrinfo(servinfo);

  if (p == NULL){
    fprintf(stderr, "[!!!] Server: failed to bind\n");
    exit(1); 
  }

  if(listen(sockfd, BACKLOG) == -1){
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchild_handler; //reap dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1){
    perror("[!!] sigaction");
    exit(1);
  }

  printf("server: establishing connection...\n");
  while(1){
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
    if (new_fd == -1){
      perror("[!!!] Could Not Accept");
      continue;
    }

    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *)&their_addr), //get incoming address
              s, sizeof s);
    
    printf("server: new connection from %s\n", s);
    break; //this might be problematic
  } 
  return 0; 


}

int sendmesg(char *msg){
  
  if (!fork()) {
    if (send(new_fd, msg, (strlen(msg)+2),0) == -1){
      perror("[!!!] could not send");
      return -1;
    }
    sleep(1);
  } 
  return 0;
}

void killinc(){
  close(sockfd);
  close(new_fd);
}

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

#define TRUE 1

#define FALSE 0

#define PORT "3490" // le beautiful port

#define BACKLOG 10 // # of pending conn queue will hold

#define NUM_THREADS 2


int opt = TRUE;

int master_socket, addrlen, new_socket, client_socket[2], max_clients = 30, activity, 
    i, valread, sd;

int max_sd;

struct addrinfo hints, *servinfo, *p; 

struct sockaddr_storage their_addr; // remotehost addr info 

socklen_t sin_size;

int yes=1;

char s[INET6_ADDRSTRLEN];

int address_info;

char buffer[1025];

char *msg;

void setmsg(char *buf){
  msg = buf;
}


void *get_in_addr(struct sockaddr *sa){
  if (sa -> sa_family == AF_INET){ //if IPv4
    return &(((struct sockaddr_in*)sa)->sin_addr);//return the addr of 'sin'
  }
  //else we have an IPv6 addr
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
 


  memset(&hints, 0, sizeof(hints)); //zero the struct
  hints.ai_family = AF_UNSPEC;    //set the type of connection to IPv4/6
  hints.ai_socktype = SOCK_STREAM;  //TCP stream sockets
  hints.ai_flags = AI_PASSIVE;    //fill in IP for me



  fd_set readfds;

  char *message = "ECHO Daemon v1.0 \r\n";

  //init client socket to 0 so not checked
  for(i = 0; i < max_clients; i++){
    client_socket[i] = 0;
  }

  //create master socket 
  if((master_socket = socket(AF_UNSPEC, SOCK_STREAM, 0)) == 0)
  {
    perror("[!!] socket failed");
    exit(EXIT_FAILURE);
  }

  //set master socket to allow multiple connections
  //(will work w/o this is a safegaurd
  

  //get addr info
  if((address_info = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(address_info));
    return 1; 
  }

  //loop through results and bind to first we can
  for(p = servinfo; p != NULL; p = p -> ai_next){
    if((master_socket = socket(p->ai_family, p->ai_socktype,   //get a socket desc
          p->ai_protocol)) == -1){
            perror("[!ERROR!!] server: socket");
           continue;
    }
  
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt,
                  sizeof(opt))<0)
    {
      perror("[!!] setsockopt");
      exit(EXIT_FAILURE);
    }

    if(bind(master_socket, p->ai_addr,p->ai_addrlen) == -1){   //bind to it
      close(master_socket);
      perror("[!ERROR!] server bind failed");
      continue;
    }

    break;
  
  }

  freeaddrinfo(servinfo);
  //reached the end of the LL :'(
  if (p == NULL){
    fprintf(stderr, "[!!!] Server: failed to bind\n");
    exit(1); 
  }

  if(listen(master_socket, BACKLOG) == -1){
    perror("listen");
    exit(EXIT_FAILURE);
  }


  printf("[server] establishing connection...\n");
  
  
  while(TRUE){
    
    sin_size = sizeof their_addr;
    FD_ZERO(&readfds);
    FD_SET(master_socket, &readfds);
    max_sd = master_socket;
    
    //add child sockets to set
    for(i = 0; i < max_clients; i++){
      sd = client_socket[i];
      //if valid socket desc add to read list
      if(sd > 0) FD_SET(sd, &readfds);
      //highest file desc #, need it for select func
      if(sd > max_sd) max_sd = sd;
    }
   //wait for activity on one of the sockets, timeout NULL, so this will wait..foreverr 0.0
   activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

   if((activity < 0) && (errno!=EINTR)) printf("[!!] select error");
  
   if(FD_ISSET(master_socket, &readfds))
   {
    
     new_socket = accept(master_socket, (struct sockaddr*)&their_addr,&sin_size);
     
     if(new_socket == -1){
      perror("[!!] Could not Accept");
      continue;
     }
   
   }
    printf("New Connection, socket fd is %d, ip is: %s, port: %d \n", new_socket, inet_ntop(their_addr.ss_family,
          get_in_addr((struct sockaddr *) &their_addr), s, sizeof s));

    //might need htis (network to printable);
  /*  inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *)&their_addr), //get incoming address
              s, sizeof s); */
  //printf("server: new connection from %s\n", s);

    if(send(new_socket, message, strlen(message), 0) != strlen(message)){
      perror("[!!] send");
    }
    puts("Welcome message sent successfully");

    for(i = 0; i < max_clients; i++){
      if(client_socket[i] == 0){
        client_socket[i] = new_socket;
        printf("Adding to list of sockets as %d\n",i);
        break;
      }
    }
  } 

  for (i = 0; i < max_clients; i++)
  {
    sd = client_socket[i];

    if(FD_ISSET(sd,&readfds))
    {
      //check if it was for closing, also read inc msg
      if ((valread = read(sd, buffer, 1024)) == 0)
      {
        //somebody d/ced get deets and p
        getpeername(sd,(struct sockaddr*)&their_addr, (&sin_size));
        
        printf("Host disconnected, ip %s , port %d \n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s)));

        close (sd);
        client_socket[i] = 0;
      }else{
        //set string terminating NULL byte on end of data read
        buffer[valread] = '\0';
        send(sd, buffer, strlen(buffer), 0);
      
      }
    }
  
  }


}
/* //play with these to send data to clients
 *
 * 'send all' b/c kernel may not send all bytes over the network
 * that is out of our control, but we need to make s ure everything makes it over
 *
//sometimes not all of the data sends >.< ....
int sendall(int s, char *buf, int *len){
  int total = 0;
  int bytesleft = *len;
  int n;
  
  while(total < *len){
    n = send(s, buf+total, bytesleft, 0);
    if(n==-1){
      perror("[!!!] could not send");
      break;} //means send failed
    total += n; //tally up what was sent
    bytesleft -= n; //bytes left to send
  }

  *len = total; //number actually sent

  return n == -1? -1:0; //return -1 on failure, 0 on success
}


int sendmesg(){
  int len = strlen(msg) + 1; 
    if (sendall(new_fd, msg, &len) == -1){
      perror("[!!!] could not send");
      return -1;
    }
    sleep(1);
  return 0;
}

void *sendmesgptr(){
  //could do an if-else error structure here, but it's covered in sendmesg() already
  sendmesg(msg);
  return NULL;
}
*/

//kill outgoing
void killout(){
  close(master_socket);
  //close client sockets here
}

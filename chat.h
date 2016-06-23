//chat.c
char *input();
char *addchar(const char *msg, const char *b);

//server.c
int init_server();

void setmsg(char *buf);
int sendmesg();
void *sendmesgptr();

void killout();

//client.c
void *initclientptr();
void killinc();
void setcuser(char *buf);

//mainthread
void *mainthreadptr();
void setuser(char *buf);



void setuser(char *buf){
  username = buf;
}

void sethost(char *buf){
  hostname = buf;
}

//An error-checked malloc() wrapper function
void *ec_malloc(unsigned int size){
  void *ptr;
  ptr = malloc(size);
  if(ptr == NULL)
    fatal("in ec_malloc() on memory allocation");
  return ptr;
}

void *ec_realloc(char* str, unsigned int size){
  void *ptr;
  ptr = realloc(str, size);
  if(ptr == NULL)
    fatal("in ec_realloc() on memory re-allocation");
  return ptr;
}

//dest src
void fatal(char *message){
  char *error_message = "[!!] Fatal Error ";
  strncat(error_message, message);
  error_message[strlen(error_message)] = '\0';
  perror(error_message);
  exit(-1);
}



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


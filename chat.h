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
void sethost(char *buf);
void killinc();

//mainthread
void *mainthreadptr();
void setuser(char *buf);

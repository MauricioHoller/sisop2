#ifndef CLIENTE_H
#define CLIENTE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <pwd.h>



#define MAXNAME 20
#define MINNAME 3





int connect_server (char *host, int port);
void login(int socket);
void* consumidor(void *args);
void* produtor(void *args);




#endif








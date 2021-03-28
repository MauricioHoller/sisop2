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
#include "profile.h"


/* User client struct */
typedef struct client {
    int socket;                         // user current socket
    pthread_t thread;                   // session thread
    pthread_mutex_t mutex;              // session socket mutex
    Profile* user_profile; // profile da sessao
    char username[MAXNAME];   // user name
    int devices[2];	//quantidade de devices logados	
} Cliente;


int connect_server (char *host, int port);

void login(int socket);

void *clientSnd(void *args);

void *clientRcv(void * args);


#endif
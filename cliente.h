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

/* User client struct */
typedef struct client {
    int socket;                         // user current socket
    pthread_t thread;                   // session thread
    pthread_mutex_t *mutex;              // session socket mutex
    char username[MAXNAME];   // user name
    int devices[2];	//quantidade de devices logados	
} Cliente;


int connect_server (char *host, int port);
void login(int socket);
void* consumidor(void *args);
void* produtor(int socket);

int verificaTipo(char *msg);



#endif








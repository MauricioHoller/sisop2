#include "cliente.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include "mensagem.h"
#include <time.h>

int readMessage(int socket, PACOTE *msg) {
    unsigned n = 0;
    int r = 0;
    char *buff = (char*) msg;
    bzero(buff, sizeof(PACOTE));
    do {
        r += read(socket, &buff[n], sizeof(PACOTE) - n);
        if(r <= 0) return r;
        n += r;
    } while(n < sizeof(PACOTE));
    msg->type = ntohs(msg->type);
    return n;
}

int sendMessage(int socket, PACOTE *msg) {
    unsigned n = 0;
    int r = 0;
    
    msg->type = htons(msg->type);
    char *buff = (char*) msg;


    do {
        r += write(socket, &buff[n], sizeof(PACOTE) - n);
        if(r <= 0) return r;
        n += r;
    } while(n < sizeof(PACOTE));


    msg->type = ntohs(msg->type);
    return n;
}

void serverMessage(PACOTE* msg, int type, char* text) {
    msg->type = type;
    strncpy(msg->username, "SERVER", strlen("SERVER"));
    strncpy(msg->txt, text, strlen(text) + 1);
}

void updateReplicaMessage(PACOTE* msg, char* text, char* sent_user, char* to_user)
{
    msg->type = REPLICA_UPDATE;

    strncpy(msg->user_sent, sent_user, strlen(sent_user) + 1);
    strncpy(msg->username, sent_user, strlen(sent_user) + 1);

    strncpy(msg -> txt, text, strlen(text) + 1);

    msg->timestamp = time(NULL);

}


void clientMessage(PACOTE* msg, int type, char* username, char* text) {
    msg->type = type;
    strncpy(msg->username, username, strlen(username) + 1);

    strncpy(msg -> txt, text, strlen(text) + 1);

    msg->timestamp = time(NULL);
    	
}





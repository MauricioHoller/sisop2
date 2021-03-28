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

    msg->dados = calloc(strlen(text)+1, sizeof(char));
    strncpy(msg->txt, text, strlen(text) + 1);
}

void clientMessage(PACOTE* msg, int type, char* username, char* text) {
    msg->type = type;
    strncpy(msg->username, username, strlen(username) + 1);
    
    msg->dados = calloc(strlen(text)+1, sizeof(char));
    strncpy(msg -> txt, text, strlen(text) + 1);

    msg->timestamp = time(NULL);
    	
}

int get_msg_type (char * token) {

    if ( strcmp(token, "FOLLOW") == 0) { 
        return  FOLLOW;
    }
    if ( strcmp(token, "SEND") == 0) { 
        return  SEND;
    }
    if ( strcmp(token, "QUIT") == 0) { 
        return  QUIT;
    }
    if ( strcmp(token, "LOGIN") == 0) { 
        return  LOGIN;
    }

    return ERRO;
}


void parse_message(char *msg_txt, PACOTE * msg)
{
    if ((strlen(msg_txt) > 0) && (msg_txt[strlen (msg_txt) - 1] == '\n'))
        msg_txt[strlen (msg_txt) - 1] = '\0';

    
    char *msg_buffer = strcpy(msg_buffer, msg_txt);

    char *token = strtok(msg_buffer, " \t");

    msg -> type = get_msg_type(token);

    if ( msg -> type == ERRO ) {
        printf("Command %s not found\n", token);
        return;
    }
   
    int copy_index = strlen(token) + 1;
    int chars_to_copy = strlen(msg_txt) - copy_index;
    strncpy(msg -> txt, msg_txt + copy_index,  chars_to_copy);

}


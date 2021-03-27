#ifndef ESTRUTURAlISTA_H
#define ESTRUTURAlISTA_H

#include <pthread.h>
#include "servidor.h"

#define MSG_MAX_SIZE 281

typedef struct client_list {
    struct client_list * next; //proximo da lista
    Cliente client; //cliente
    pthread_mutex_t mutex; // mutex do cliente atual
} Client_list;


typedef struct notification_list {
    struct notification_list * next;

    char notification_txt[MSG_MAX_SIZE];    
    char ** users_to_notify;
    
} NotificationList;

void newList(struct client_list *client_list);
void insertList(struct client_list **client_list, struct client client);
int isEmpty(struct client_list *client_list);
int findNode(char *userid, struct client_list *client_list, struct client_list **client_node);




#endif








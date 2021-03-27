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

    char * notification_txt;    
    char ** users_to_notify;
    
} NotificationList;



// Client list functions
void new_client_list(struct client_list *client_list);

void insert_client_list(struct client_list **client_list, struct client client);

int  is_client_list_empty(struct client_list *client_list);

int  find_client_node(char *userid, struct client_list *client_list, struct client_list **client_node);

// Notification list functions
NotificationList* new_notification_list(char* notification_txt, char ** users_notify);

void insert_notification_list(NotificationList * current_list, NotificationList* insert_list);



#endif
#include "estruturaListas.h"
#include "servidor.h"
#include <stdlib.h>
#include <string.h>



void new_client_list(struct client_list *client_list)
{
	client_list = NULL;
}

void insert_client_list(struct client_list **client_list, struct client client)
{
	struct client_list *client_node;
	struct client_list *client_list_aux = *client_list;

	client_node = malloc(sizeof(struct client_list));

	client_node->client = client;
	client_node->next = NULL;

	if (*client_list == NULL)
	{
		*client_list = client_node;
	}
	else
	{
		while(client_list_aux->next != NULL)
			client_list_aux = client_list_aux->next;

		client_list_aux->next = client_node;
	}
}

int is_client_list_empty(struct client_list *client_list)
{
	if (client_list== NULL)
		return 0;
	
	return 1;

}

int find_client_node(char *userid, struct client_list *client_list, struct client_list **client_node)
{
	struct client_list *client_list_aux = client_list;
	
	while(client_list_aux != NULL)
	{
		if (strcmp(userid, client_list_aux->client.username) == 0)
		{
			*client_node = client_list_aux;
			return 1;
		}
		else
			client_list_aux = client_list_aux->next;
	}
	return 0;
}

NotificationList *new_notification_list(char* notification_txt, char ** users_notify)
{

    NotificationList *n_list = (NotificationList *)malloc(sizeof(NotificationList));

	if (notification_txt != NULL && users_notify != NULL){ 
		strcpy(n_list -> notification_txt, notification_txt);
		memcpy(n_list -> users_to_notify, users_notify, sizeof(users_notify));
		return n_list;
	}

	n_list -> notification_txt = NULL;
	n_list -> users_to_notify = NULL;
	
    return n_list;
}

void insert_notification_list(NotificationList * current_list, NotificationList* insert_list) {



	return;
}
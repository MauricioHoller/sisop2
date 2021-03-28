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

NotificationList *new_notification_list()
{

    NotificationList *n_list = (NotificationList *)malloc(sizeof(NotificationList));


	n_list -> notification = NULL;
	n_list -> users_to_notify = NULL;

    return n_list;
}

void insert_notification(NotificationList * n_list, Notification* new_notification) {
	
	NotificationList *current = n_list;

    if (n_list -> notification == NULL)
    {

        n_list->notification = new_notification;
        n_list->next = new_notification_list();
        return;
    }

    insert_notification (n_list->next, new_notification);

}
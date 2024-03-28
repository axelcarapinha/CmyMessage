#include "queue.h"

node_t *head = NULL;
node_t *tail = NULL;

bool isEmptyQueue() {
    return head == NULL;
}

void enqueue(ClientInfo *client_struct_ptr)
{
    node_t *new_node = malloc(sizeof(ClientInfo *));
    new_node->client_struct_ptr = client_struct_ptr;
    new_node->next = NULL;
    if (tail == NULL) {
        head = new_node;
    } else {
        tail->next = new_node;
    }
    tail = new_node;
}

ClientInfo* dequeue() {
    if (isEmptyQueue()) 
        return NULL;
    
    ClientInfo *result = head->client_struct_ptr;

    // Advance to the next memory address
    node_t *temp = head;
    head = head->next;

    if (isEmptyQueue()) 
        tail == NULL;
    
    free(temp);
    return result;
}


#ifndef QUEUE_H
#define QUEUE_H

#include "net_utils_tcp.h"

struct node
{
    struct node *next;
    ClientInfo *client_struct_ptr;
};
typedef struct node node_t;

// Functions
ClientInfo* dequeue();
void enqueue(ClientInfo *client_struct_ptr);
bool isEmptyQueue();




#endif
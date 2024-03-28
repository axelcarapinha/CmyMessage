#ifndef QUEUE_H
#define QUEUE_H

#include "net_utils_tcp.h"

#define MAX_QUEUE_SIZE (SIZE_THREAD_POOL * 3)

struct node
{
    struct node *next;
    ClientInfo *client_struct_ptr;
};
typedef struct node node_t;

// Functions
ClientInfo* dequeue();
int enqueue(ClientInfo *client_struct_ptr);
bool isEmptyQueue();




#endif
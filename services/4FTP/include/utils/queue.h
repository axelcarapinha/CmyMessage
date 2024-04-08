#ifndef QUEUE_H
#define QUEUE_H

#include "net_utils_tcp.h"

#define MAX_QUEUE_SIZE (SIZE_THREAD_POOL * 3)

struct node
{
    struct node *next;
    ClientInfo_t *p_client_t;
};
typedef struct node node_t;

// Functions
ClientInfo_t* dequeue();
int enqueue(ClientInfo_t *client_struct_ptr);
bool isEmptyQueue();


#endif
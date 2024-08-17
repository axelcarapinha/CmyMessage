/*
 * Purpose: Data Structure for implementations
 */

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

/**
 * @brief 
 * 
 * @return ClientInfo_t* 
 */
ClientInfo_t* dequeue();

/**
 * @brief Adds an element to the queue,
 * if it has space for it
 * 
 * @param p_client_t 
 * @return int 
 */
int enqueue(ClientInfo_t *client_struct_ptr);

/**
 * @brief Checks if the queue is empty or not
 * 
 * @return true 
 * @return false 
 */
bool isEmptyQueue();


#endif
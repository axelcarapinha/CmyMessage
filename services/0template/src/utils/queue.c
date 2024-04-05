#include "queue.h"

int queue_size = 0;
node_t *head = NULL;
node_t *tail = NULL;

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param
 * @return
 */
bool isEmptyQueue() { return head == NULL; }

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param
 * @return
 */
int enqueue(ClientInfo_t *p_client_t)
{
    // Alert the client if the server is full of capacity for now
    if (queue_size >= SIZE_THREAD_POOL)
    {
        char *message;

        if (queue_size == MAX_QUEUE_SIZE)
        {
            message = "Sorry, we are having too much requests. Try again later.";
            send(p_client_t->sock_FD, message, strlen(message), 0);
            ERROR_VERBOSE_LOG("Queue size is too large. The client must wait");
            return EXIT_FAILURE;
        }

        message = "Request pending... We thank your patience :)";
        send(p_client_t->sock_FD, message, strlen(message), 0);
    }

    // Add the client to the pending requests
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL)
    {
        ERROR_VERBOSE_LOG("Memory allocation for the new node failed");
        return -1;
    }
    //
    new_node->p_client_t = p_client_t;
    new_node->next = NULL;
    if (tail == NULL)
    {
        head = new_node;
    }
    else
    {
        tail->next = new_node;
    }
    tail = new_node;

    // Update the size of the queue
    queue_size++;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param
 * @return
 */
ClientInfo_t *dequeue()
{
    if (isEmptyQueue())
        return NULL;

    ClientInfo_t *result = head->p_client_t;

    // Advance to the next memory address
    node_t *temp = head;
    head = head->next;

    if (isEmptyQueue())
        tail = NULL;

    // Update the size of the queue
    queue_size--;

    free(temp);
    return result;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param
 * @return
 */
// TODO clean all the queue
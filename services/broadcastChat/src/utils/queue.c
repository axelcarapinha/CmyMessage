#include "queue.h"

int queue_size = 0;
node_t *head = NULL;
node_t *tail = NULL;

bool isEmptyQueue()
{
    return head == NULL;
}

int enqueue(ClientInfo *client_struct_ptr)
{
    // Alert the client if the server is full of capacity for now
    if (queue_size >= SIZE_THREAD_POOL) {
        char *message;

        if (queue_size == MAX_QUEUE_SIZE) {
            message = "Sorry, we are having too much requests. Try again later.";
            send(client_struct_ptr->client_handler_FD, message, strlen(message), 0);
            fprintf(stderr, "Queue size is too large. The client must wait.");
            return EXIT_FAILURE;
        }
        
        message = "Request pending... We thank your patience :)";
        send(client_struct_ptr->client_handler_FD, message, strlen(message), 0);   
    }

    // Add the client to the pending requests
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation for the new node failed.\n");
        return EXIT_FAILURE;
    }
    //
    new_node->client_struct_ptr = client_struct_ptr;
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

ClientInfo *dequeue()
{
    if (isEmptyQueue())
        return NULL;

    ClientInfo *result = head->client_struct_ptr;

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

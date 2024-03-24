#include "service_utils.h"

/**
 * @brief
 *
 * @param a
*/
void close_service(uniSocket *server_struct_ptr) {
    close_server_socket(server_struct_ptr);
    puts("Service closed.");
}


/**
 * @brief
 *
 * @param a
*/
void handle_thread_creation_and_exit(pthread_t *thread_id_ptr, int thread_create_status)
{
    if (thread_create_status != 0)
    {
        fprintf(stderr, "Value return from the thread creation is %d\n", thread_create_status);
        return;
    }

    // Checking for THREAD JOINING errors
    void *status_ptr; // pthread_join already allocates memory for the pointer
    int join_status = pthread_join(*thread_id_ptr, &status_ptr);
    if (join_status != 0)
    {
        fprintf(stderr, "Error joining listener thread: %d\n", join_status);
        return;
    }

    // Checking for EXECUTION errors
    if (status_ptr != NULL)
    {
        int *status_val = (int *)status_ptr;
        if (*status_val != 0)
        {
            fprintf(stderr, "Listener thread returned value %d\n", *status_val);
        }
    }

    // Avoid leakage of memory allocated by the pthread_join function
    free(status_ptr);
}

/**
 * @brief
 *
 * @param a
*/
void accept_incoming_connections(void *server_struct_ptr_arg)
{
    uniSocket *server_struct_ptr = (uniSocket *)server_struct_ptr_arg;
    while (true)
    {
        long *client_handler_ptr = (long *)malloc(sizeof(long));
        if (client_handler_ptr == NULL)
        {
            perror("Problem allocating memory for the client_handler_ptr");
            exit(EXIT_FAILURE);
        }

        *client_handler_ptr = acceptConnection(server_struct_ptr->sock_fd,
                                               (struct sockaddr *)server_struct_ptr->address.addr_ipv4,
                                               &(server_struct_ptr->addrlen));
        if (*client_handler_ptr < 0)
        {
            perror("Error accepting connection");
            free(client_handler_ptr);
            continue;
        }

        // Handle requests in a separate thread
        pthread_t handler_thread;
        int thread_creation_status = pthread_create(&handler_thread, NULL, 
            (void *(*)(void *))server_struct_ptr->service_function_ptr, 
            (void *)client_handler_ptr);

        handle_thread_creation_and_exit(&handler_thread, thread_creation_status);
    }
}

/**
 * @brief
 *
 * @param a
*/
void start_accepting_incoming_connections(uniSocket *server_struct_ptr)
{
    // In the end, the MAX_NUMBER_OF_CONNECTIONS handles the amount of threading
    pthread_t listening_thread;
    int thread_creation_status;
    thread_creation_status = pthread_create(&listening_thread, NULL,
                                            (void *(*)(void *))accept_incoming_connections, (void *)server_struct_ptr);

    handle_thread_creation_and_exit(&listening_thread, thread_creation_status);
}

/**
 * @brief
 *
 * @param a
*/
void start_service(int port, ServiceFunctionPtr service_function_ptr)
{
    uniSocket *server_struct_ptr = create_socket(true, port, true);
    server_struct_ptr->service_function_ptr = service_function_ptr;

    start_accepting_incoming_connections(server_struct_ptr);

    close_service(server_struct_ptr);
}
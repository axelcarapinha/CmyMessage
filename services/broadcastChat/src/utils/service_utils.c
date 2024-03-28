#include "service_utils.h"
#include <sys/select.h>

/**
 * @brief
 *
 * @param a
 */
void close_service(uniSocket *server_struct_ptr)
{
    close_server_socket(server_struct_ptr);
    puts("Service closed.");
}

/**
 * @brief
 *
 * @param a
 */
void join_thread_and_handle_errors(pthread_t *thread_id_ptr)
{
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
void handle_thread_creation_and_exit(int thread_create_status)
{
    if (thread_create_status != 0)
    {
        fprintf(stderr, "Value return from the thread creation is %d\n", thread_create_status);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Thread created successfully\n");
    }
}

/**
 * @brief
 *
 * @param a
 */
void* search_for_thread_work(void *server_struct_ptr_arg)
{
    uniSocket *server_struct_ptr = (uniSocket *)server_struct_ptr_arg;
    pthread_mutex_t *queue_mutex_ptr = server_struct_ptr->thread_mutex_queue_ptr;

    // Keep waiting for a client to handle
    while (true)
    {
        if (!isEmptyQueue())
        {
            pthread_mutex_lock(queue_mutex_ptr);
            ClientInfo *client_struct_ptr = dequeue();
            pthread_mutex_unlock(queue_mutex_ptr);

            // Forward the client to the service function
            void (*functionPtr)(ClientInfo *) = client_struct_ptr->service_function_ptr;

            //TODO retirar isto
            const char *message = "hello from the thread\n";
            send(client_struct_ptr->client_handler_FD, message, strlen(message), 0);

            (*functionPtr)(client_struct_ptr);

            // Deallocation of the client's memory struct
            free(client_struct_ptr);
        }
        else {
           sleep(3); //TODO retirar isto
        }
    }
}

/**
 * @brief
 *
 * @param a
 */
void accept_incoming_connections(void *server_struct_ptr_arg)
{
    uniSocket *server_struct_ptr = (uniSocket *)server_struct_ptr_arg;
    pthread_mutex_t *queue_mutex_ptr = server_struct_ptr->thread_mutex_queue_ptr;

    while (true)
    {
        ClientInfo *client_struct_ptr = acceptConnection(server_struct_ptr->sock_fd);
        if (client_struct_ptr == NULL)
        {
            printf("Error allocating memory for the client info struct");
            continue;
        }

        // Assign the service to the client
        client_struct_ptr->service_function_ptr = server_struct_ptr->service_function_ptr;

        // TODO handle messages to the client while the queue is not available (and 
        // Assign the client to an available thread
        pthread_mutex_lock(queue_mutex_ptr);
        enqueue(client_struct_ptr);
        pthread_mutex_unlock(queue_mutex_ptr);
    }

    // TODO
    //  free(client_handler_ptr);
}

/**
 * @brief
 *
 * @param a
 */
void start_accepting_incoming_connections(uniSocket *server_struct_ptr)
{
    // Prepare the thread pool
    pthread_t thread_pool[SIZE_THREAD_POOL];
    server_struct_ptr->thread_pool = thread_pool;

    // Create the lock for the client's queue
    pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    server_struct_ptr->thread_mutex_queue_ptr = &queue_mutex;

    //
    for (int i = 0; i < SIZE_THREAD_POOL; i++)
    {
        int thread_creation_status;
        thread_creation_status = pthread_create(&thread_pool[i], NULL, (void *(*)(void *))search_for_thread_work, (void *)server_struct_ptr);
        handle_thread_creation_and_exit(thread_creation_status);

        //TODO aqui alterei
        // join_thread_and_handle_errors(&thread_pool[i]);
    }

    // Start listening for connections on a separate thread
    pthread_t listening_thread;
    int thread_creation_status = pthread_create(&listening_thread, NULL, (void *(*)(void *))accept_incoming_connections, (void *)server_struct_ptr);
    //
    handle_thread_creation_and_exit(thread_creation_status);
    join_thread_and_handle_errors(&listening_thread);
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

    // TODO return an exit status
    close_service(server_struct_ptr);
}
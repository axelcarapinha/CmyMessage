#include "service_utils.h"
#include <sys/select.h>


/**
 * @brief
 *
 * @param a
 */
void close_service(UniSocket_t *p_server_t)
{
    close_server_socket(p_server_t);
    puts("Service closed.");
}

/**
 * @brief
 *
 * @param a
 */
void join_thread_and_handle_errors(pthread_t *thread_id_ptr)
{
    void *status_ptr; // pthread_join already allocates memory for the pointer
    int join_status = pthread_join(*thread_id_ptr, &status_ptr);
    if (join_status != 0)
    {
        fprintf(stderr, "Error joining listener thread: %d\n", join_status);
        return;
    }
    //
    if (status_ptr != NULL)
    {
        int *status_val = (int *)status_ptr;
        if (*status_val != 0)
        {
            fprintf(stderr, "Listener thread returned value %d\n", *status_val);
        }
    }
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
void *search_for_thread_work(void *p_server_t_arg)
{
    UniSocket_t *p_server_t = (UniSocket_t *)p_server_t_arg;
    pthread_mutex_t *queue_mutex_ptr = p_server_t->p_mutex_queue;
    pthread_cond_t *condition_var_ptr = p_server_t->p_condition_var;

    // Keep waiting for a client to handle
    while (true)
    {
        if (!isEmptyQueue())
        {

            // Safely search for incoming client requests
            pthread_mutex_lock(queue_mutex_ptr);
            ClientInfo_t *client_struct_ptr;
            if ((client_struct_ptr = dequeue()) == NULL)
            {
                pthread_cond_wait(condition_var_ptr, queue_mutex_ptr);

                // Avoid potential deadlock
                client_struct_ptr = dequeue();
            }
            pthread_mutex_unlock(queue_mutex_ptr);

            // Handle client request
            if (client_struct_ptr != NULL)
            {
                // Forward the client to the service function
                void (*functionPtr)(ClientInfo_t *) = p_client_t->p_service_func;
                (*functionPtr)(client_struct_ptr);

                // Deallocation of the client's memory struct
                free(client_struct_ptr);
            }
        }
    }
}

/**
 * @brief
 *
 * @param a
 */
void accept_incoming_connections(void *p_server_t_arg)
{
    UniSocket_t *p_server_t = (UniSocket_t *)p_server_t_arg;
    pthread_mutex_t *queue_mutex_ptr = p_server_t->p_mutex_queue;
    pthread_cond_t *condition_var_ptr = p_server_t->p_condition_var;

    while (true)
    {
        ClientInfo_t *client_struct_ptr = acceptConnection(p_server_t->sock_fd);
        if (client_struct_ptr == NULL)
        {
            printf("Error allocating memory for the client info struct");
            continue;
        }

        // Assign the service to the client
        client_struct_ptr->p_server_t = p_server_t->p_server_t;

        // TODO handle messages to the client while the queue is not available (and
        // Assign the client to an available thread
        pthread_mutex_lock(queue_mutex_ptr);
        enqueue(client_struct_ptr);
        pthread_cond_signal(condition_var_ptr);
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
void start_accepting_incoming_connections(UniSocket_t *p_server_t)
{
    // Prepare the thread pool
    pthread_t thread_pool[SIZE_THREAD_POOL];
    p_server_t->thread_pool = thread_pool;

    // Create the lock for the client's queue
    pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    p_server_t->p_mutex_queue = &queue_mutex;

    // Create the conditional variable for the threads
    pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
    p_server_t->p_condition_var = &condition_var;

    //
    for (int i = 0; i < SIZE_THREAD_POOL; i++)
    {
        int thread_creation_status;
        thread_creation_status = pthread_create(&thread_pool[i], NULL, (void *(*)(void *))search_for_thread_work, (void *)p_server_t);
        handle_thread_creation_and_exit(thread_creation_status);
    }

    // Start listening for connections on a separate thread
    pthread_t listening_thread;
    int thread_creation_status = pthread_create(&listening_thread, NULL, (void *(*)(void *))accept_incoming_connections, (void *)p_server_t);
    //
    handle_thread_creation_and_exit(thread_creation_status);
    join_thread_and_handle_errors(&listening_thread);
}

/**
 * @brief
 *
 * @param a
 */
void start_service(int port, ServiceFunctionPtr p_server_t)
{
    UniSocket_t *p_server_t;
     = create_socket(true, port, true);
    p_server_t->p_server_t = p_server_t;

    start_accepting_incoming_connections(p_server_t);

    close_service(p_server_t);
}
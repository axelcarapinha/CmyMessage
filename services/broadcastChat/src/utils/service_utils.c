#include "service_utils.h"
#include <sys/select.h>

// Global variable to support concurrency in the access
// of the server structure
static pthread_mutex_t g_mutex_server = PTHREAD_MUTEX_INITIALIZER;

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param a
 */
void close_service(UniSocket_t *p_server_t)
{
    // Signal all the threads to finish
    pthread_mutex_lock(&g_mutex_server);
    *(p_server_t->p_quit_signal) = CLOSE_SERVER;
    pthread_mutex_unlock(&g_mutex_server);

    close_server_socket(p_server_t);
    puts("Service closed.");
}
//TODO consider using a cleanup handler for the threads 
//TODO to dispose already connected clients

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param a
 */
int join_thread_and_handle_errors(pthread_t *p_thread_ID)
{
    void *p_thread_return_val;
    int join_status;
    if ((join_status = pthread_join(*p_thread_ID, &p_thread_return_val)) < 0)
    {
        perror("Error joining listener thread");
        return join_status;
    }
    //
    if (p_thread_return_val == NULL)
    {
        perror("pthread_join could not allocate memory for the thread return value");
        return -1;
    }
    else
    {
        int thread_return_val = *(int *)p_thread_return_val;
        if (thread_return_val < 0)
        {
            fprintf(stderr, "Thread with id %lu returned %d", *p_thread_ID, thread_return_val);
        }
        return thread_return_val;
    }
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param a
 */
void *search_for_thread_work(void *p_server_t_arg)
{
    UniSocket_t *p_server_t = (UniSocket_t *)p_server_t_arg;

    // Get the needed values from the (shared by threads) server struct
    pthread_mutex_lock(&g_mutex_server);
    pthread_mutex_t *p_queue_mutex = p_server_t->p_mutex_queue;
    pthread_cond_t *p_condition_var = p_server_t->p_condition_var;
    //
    pthread_mutex_t *p_mutex_quit_signal = p_server_t->p_mutex_quit_signal;
    volatile sig_atomic_t *p_quit_signal = p_server_t->p_quit_signal;
    pthread_mutex_unlock(&g_mutex_server);

    int quit_signal_val;
    do
    {
        // CHECK for a client request
        if (!isEmptyQueue())
        {
            pthread_mutex_lock(p_queue_mutex);
            ClientInfo_t *p_client_t;
            if ((p_client_t = dequeue()) == NULL)
            {
                pthread_cond_wait(p_condition_var, p_queue_mutex);
                p_client_t = dequeue(); // Avoid potential deadlock
            }
            pthread_mutex_unlock(p_queue_mutex);

            // HANDLE client request
            if (p_client_t != NULL)
            {
                // Forward the client to the desired service function
                void (*functionPtr)(ClientInfo_t *) = p_client_t->p_service_func;
                (*functionPtr)(p_client_t);

                free_client_memory(p_client_t);
            }
        }

        // Check if the service was requested to close
        // TODO by
        pthread_mutex_lock(p_mutex_quit_signal);
        quit_signal_val = *p_quit_signal;
        pthread_mutex_unlock(p_mutex_quit_signal);

    } while (!quit_signal_val);
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param a
 */
void accept_incoming_connections(void *p_server_t_arg)
{
    UniSocket_t *p_server_t = (UniSocket_t *)p_server_t_arg;

    // Get the needed values from the (shared by threads) server struct
    pthread_mutex_lock(&g_mutex_server);
    pthread_mutex_t *p_queue_mutex = p_server_t->p_mutex_queue;
    pthread_mutex_t *p_mutex_quit_signal = p_server_t->p_mutex_quit_signal;
    volatile sig_atomic_t *p_quit_signal = p_server_t->p_quit_signal;
    pthread_cond_t *p_condition_var = p_server_t->p_condition_var;
    //
    int server_FD = p_server_t->sock_FD;
    ServiceFunctionPtr p_service_func = p_server_t->p_service_func;
    pthread_mutex_unlock(&g_mutex_server);

    ClientInfo_t *p_client_t;
    int quit_signal_val;
    do
    {
        if ((p_client_t = accept_connection(server_FD)) == NULL)
        {
            perror("Error allocating memory for the client info struct");
            continue; // another thread will PROBABLY handle the client
        }

        // Assign the service to the client
        p_client_t->p_service_func = p_service_func;

        // Assign the client to an available thread
        pthread_mutex_lock(p_queue_mutex);
        enqueue(p_client_t);
        pthread_cond_signal(p_condition_var);
        pthread_mutex_unlock(p_queue_mutex);

        // Check if the service was requested to close
        // TODO by
        pthread_mutex_lock(p_mutex_quit_signal);
        quit_signal_val = *p_quit_signal;
        pthread_mutex_unlock(p_mutex_quit_signal);

    } while (!quit_signal_val); //

    // Gracefully detach clients from the connection
    pthread_mutex_lock(p_queue_mutex);
    const service_quit_message = "The service closed. Try again later. Sorry for any caused inconvenient.";
    while (!isEmptyQueue())
    {
        p_client_t = dequeue();
        send(p_client_t->sock_FD, service_quit_message, strlen(service_quit_message), 0);
        free_client_memory(p_client_t);
    }
    pthread_mutex_unlock(p_queue_mutex);
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param a
 */
int start_accepting_incoming_connections(UniSocket_t *p_server_t)
{
    // For concurrency
    pthread_t p_thread_pool[SIZE_THREAD_POOL];
    p_server_t->p_thread_pool = p_thread_pool;
    //
    pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
    p_server_t->p_condition_var = &condition_var;
    //
    pthread_mutex_t mutex_queue = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mutex_quit_signal = PTHREAD_MUTEX_INITIALIZER;
    p_server_t->p_mutex_queue = &mutex_queue;
    p_server_t->p_mutex_quit_signal = &mutex_quit_signal;
    //
    volatile sig_atomic_t quit_signal = 0; // to finish threads gracefully
    p_server_t->p_quit_signal = &quit_signal;
    // Volatile avoids processor optimizations
    // allowing changes from signal handlers
    // to be always considered

    for (int i = 0; i < SIZE_THREAD_POOL; i++)
    {
        int creation_status;
        creation_status = pthread_create(p_thread_pool[i], NULL, (void *(*)(void *))search_for_thread_work, (void *)p_server_t);
        if (creation_status < 0)
        {
            perror("Error creating one of the pool threads of the server");
            continue;
        }
    }

    // Start listening for connections on a separate thread
    pthread_t listening_thread;
    int creation_status;
    if ((creation_status = pthread_create(&listening_thread, NULL, (void *(*)(void *))accept_incoming_connections, (void *)p_server_t)) < 0)
    {
        perror("Error creating the listening thread for the server");
        return creation_status;
    }
    //
    int join_status;
    if ((join_status = join_thread_and_handle_errors(&listening_thread)) < 0)
    {
        perror("Error joinin listening thread of the server");
        return join_status;
    }
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param a
 */
int start_service(int port, ServiceFunctionPtr p_server_t)
{
    UniSocket_t *p_server_t;
    if ((p_server_t = create_socket(true, port, true) == NULL))
    {
        perror("Error getting the socket struct pointer");
        return NULL;
    }

    // TODO error values to return
    int error_status;
    if ((error_status = start_accepting_incoming_connections(p_server_t)) < 0) {
        perror("Error acepting incoming connections");
        return error_status;
    }
    
    close_service(p_server_t);
}
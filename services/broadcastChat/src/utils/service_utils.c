#include "service_utils.h"
#include <sys/select.h>

// Global variable to support concurrency in the access
// of the server structure
static pthread_mutex_t g_mutex_server = PTHREAD_MUTEX_INITIALIZER;

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Closes the service
 *
 * This function closes the service by signaling all threads to finish
 * and then closing the server socket.
 * In more detail: it sets the quit signal to indicate
 * that the server is closing, releases resources associated with the server socket,
 * and prints a message indicating that the service has been closed.
 *
 * @param p_server_t Pointer to the UniSocket_t structure representing the server
 *
 * @return None
 */
void close_service(UniSocket_t *p_server_t)
{
    // Signal all the threads to finish
    pthread_mutex_lock(&g_mutex_server);
    *(p_server_t->p_quit_signal) = CLOSE_SERVER;
    pthread_mutex_unlock(&g_mutex_server);

    close_server_socket(p_server_t);
    printf("Service closed.\n");
}
// TODO consider using a cleanup handler for the threads
// TODO to dispose already connected clients

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Joins a thread and handles errors
 *
 * This function joins the specified thread and handles any errors that may occur during joining.
 * It checks if the thread join was successful and whether memory allocation for the thread return value failed.
 * If the thread join fails, it prints an error message and returns the join status.
 * If memory allocation for the thread RETURN value fails, it prints an error message and returns -1.
 * Otherwise, it returns the return value of the joined thread.
 *
 * @param p_thread_ID Pointer to the ID of the thread to join
 *
 * @return 0 on success, or a negative value indicating an error
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
 * @brief Searches for thread work
 *
 * This function searches for work to be done by the thread.
 * At first, it retrieves necessary values from the server structure shared by threads,
 * such as mutexes, condition variables, and quit signals.
 * After that, it continuously checks for pending client requests in the queue,
 * dequeues clients for processing, and forwards them to the desired service function.
 * It also checks for signals to close the service, and if requested, terminates its execution
 * freeing allocated resources first.
 *
 * @param p_server_t_arg Pointer to the UniSocket_t structure representing the server
 *
 * @return None
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
                int (*functionPtr)(ClientInfo_t *) = p_client_t->p_service_func;
                (*functionPtr)(p_client_t);

                free_client_memory_with_ptr_to_ptr(&p_client_t); // in case the service did NOT
            }
        }

        // Check if the service was requested to close by other thread
        pthread_mutex_lock(p_mutex_quit_signal);
        quit_signal_val = *p_quit_signal;
        pthread_mutex_unlock(p_mutex_quit_signal);

    } while (!quit_signal_val);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Gracefully detaches clients from the connection
 *
 * This function gracefully detaches PENDING clients from the connection by sending them
 * a service quit message and freeing their memory. It locks the queue mutex to
 * safely access and manipulate the client queue. It iterates through the queue,
 * dequeues each client, sends them the quit message, and frees their memory.
 * If any error occurs during dequeuing, it prints an error message.
 *
 * @param p_client_t Pointer to the ClientInfo_t structure representing the client
 * @param p_queue_mutex Pointer to the mutex for the client queue
 *
 * @return None
 */

void gracefully_reject_pending_clients(ClientInfo_t *p_client_t, pthread_mutex_t *p_queue_mutex)
{
    pthread_mutex_lock(p_queue_mutex);

    const char *service_quit_message = "The service closed. Try again later. Sorry for any caused inconvenient.";
    while (!isEmptyQueue())
    {
        p_client_t = dequeue();
        if (p_client_t != NULL)
        {
            send(p_client_t->sock_FD, service_quit_message, strlen(service_quit_message), 0);
            free_client_memory_with_ptr_to_ptr(&p_client_t);
        }
        else
        {
            perror("Error when dequeuing the client, found a null value when alerting the clients of closing the server");
        }
    }

    pthread_mutex_unlock(p_queue_mutex);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Accepts incoming connections
 *
 * This function accepts incoming connections on the server socket.
 * It retrieves necessary values from the server structure shared by threads,
 * such as mutexes, condition variables, the server socket descriptor, and the service function pointer.
 * It continuously accepts connections, assigns the service function to the client,
 * enqueues the client for processing, and checks for signals to close the service.
 * If the service is requested to close, it empties the queue of PENDING connection requests
 * using the gracefully_reject_pending_clients function.
 *
 * @param p_server_t_arg Pointer to the UniSocket_t structure representing the server
 *
 * @return None
 */
void *accept_incoming_connections(void *p_server_t_arg)
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

        // Check if the service was requested to close by other thread
        pthread_mutex_lock(p_mutex_quit_signal);
        quit_signal_val = *p_quit_signal;
        pthread_mutex_unlock(p_mutex_quit_signal);

    } while (!quit_signal_val);

    gracefully_reject_pending_clients(p_client_t, p_queue_mutex);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Initializes server concurrency and thread pool
 *
 * This function initializes concurrency mechanisms and a thread pool for the server.
 * It sets up a thread pool array, condition variables, mutexes, and a volatile quit signal
 * for handling graceful thread termination. Additionally, it creates threads for the thread pool
 * and assigns work using the search_for_thread_work function.
 *
 * @param p_server_t Pointer to the UniSocket_t structure representing the server
 *
 * @return None
 */
void initialize_server_concurrency_and_thread_pool(UniSocket_t *p_server_t)
{

    // Thread pool
    pthread_t *p_thread_pool = malloc(SIZE_THREAD_POOL * sizeof(pthread_t));
    if (p_thread_pool == NULL)
    {
        perror("Error allocating memory for thread pool");
        exit(EXIT_FAILURE);
    }
    p_server_t->p_thread_pool = p_thread_pool;

    // Conditional variables
    pthread_cond_t *p_condition_var = malloc(sizeof(pthread_cond_t));
    if (p_condition_var == NULL)
    {
        perror("Error allocating memory for condition variable");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(p_condition_var, NULL) != 0)
    {
        perror("Error initializing condition variable");
        free(p_condition_var); // Free allocated memory
        exit(EXIT_FAILURE);
    }
    p_server_t->p_condition_var = p_condition_var;

    // Mutexes
    pthread_mutex_t *p_mutex_queue = malloc(sizeof(pthread_mutex_t));
    if (p_mutex_queue == NULL)
    {
        perror("Error allocating memory for the QUEUE mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(p_mutex_queue, NULL) != 0)
    {
        perror("Error initializing the QUEUE mutex");
        free(p_mutex_queue); 
        exit(EXIT_FAILURE);
    }
    p_server_t->p_mutex_queue = p_mutex_queue;
    //
    pthread_mutex_t *p_mutex_quit_signal = malloc(sizeof(pthread_mutex_t));
    if (p_mutex_quit_signal == NULL)
    {
        perror("Error allocating memory for the QUIT SIGNAL mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(p_mutex_quit_signal, NULL) != 0)
    {
        perror("Error initializing the QUIT SIGNAL mutex");
        free(p_mutex_quit_signal); 
        exit(EXIT_FAILURE);
    }
    p_server_t->p_mutex_quit_signal = p_mutex_quit_signal;
    // TODO consdier functin for this

    // Quit signal (to finish threads gracefully)
    volatile sig_atomic_t *p_quit_signal = malloc(sizeof(sig_atomic_t));
    if (p_quit_signal == NULL)
    {
        perror("Error allocating memory for quit signal");
        exit(EXIT_FAILURE);
    }
    p_server_t->p_quit_signal = p_quit_signal; 
    *p_server_t->p_quit_signal = 0;
    // Volatile avoids processor optimizations
    // allowing changes from signal handlers
    // to be always considered

    // Creation of the thread pool
    for (int i = 0; i < SIZE_THREAD_POOL; i++)
    {
        int creation_status;
        creation_status = pthread_create(&p_thread_pool[i], NULL, (void *(*)(void *))search_for_thread_work, (void *)p_server_t);
        if (creation_status < 0)
        {
            perror("Error creating one of the pool threads of the server");
            continue;
        }
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Starts accepting incoming connections
 *
 * This function starts listening for incoming connections on a separate thread.
 * It creates a thread for listening using the accept_incoming_connections function
 * and handles any errors that may occur during thread creation or joining.
 *
 * @param p_server_t Pointer to the UniSocket_t structure representing the server
 *
 * @return 0 on success, or a negative value indicating an error
 */
int start_accepting_incoming_connections(UniSocket_t *p_server_t)
{
    initialize_server_concurrency_and_thread_pool(p_server_t);

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
 * @brief Starts the service
 *
 * This function starts the service on the specified port by creating a server socket,
 * assigning the service function, and initiating the process of accepting incoming connections.
 * It handles any errors that may occur during the creation of the socket or the start of accepting connections.
 * After the service has been started, it calls close_service to clean up resources when the service ends
 * or is requested to end.
 *
 * @param port The port number for the service
 * @param p_service_func_arg Pointer to the function that implements the service functionality
 *
 * @return 0 on success, or a negative value indicating an error
 */
int start_service(int port, ServiceFunctionPtr p_service_func_arg)
{
    // Create the server socket
    UniSocket_t *p_server_t;
    if (((p_server_t = create_socket_struct(true, port, true)) == NULL))
    {
        perror("Error getting the socket struct pointer");
        return -1;
    }

    // Assign the service function
    p_server_t->p_service_func = p_service_func_arg;

    // Start the server
    int error_status;
    if ((error_status = start_accepting_incoming_connections(p_server_t)) < 0)
    {
        perror("Error acepting incoming connections");
        return error_status;
    }

    close_service(p_server_t);
}
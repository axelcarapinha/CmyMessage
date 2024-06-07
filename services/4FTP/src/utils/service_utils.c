#include "service_utils.h"

// Global variable to support concurrency in the access of the server structure
static pthread_mutex_t g_mutex_server = PTHREAD_MUTEX_INITIALIZER;

//! Equal to the implementation in client.c
//! but not enough similarities for now to make a separate file
int fill_buffer_with_response(ClientInfo_t *p_client_t) {
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    ssize_t bytes_received;
    if ((bytes_received = recv(p_client_t->sock_FD, p_client_t->buffer, BUFFER_SIZE, 0)) < 0)
    {
        ERROR_VERBOSE_LOG("Error receiving the preferred name from the client");
        return -1;
    }
    else if (bytes_received == 0)
    {
        printf("Client terminated the connection.\n");
        return -2; 
    }

    p_client_t->buffer[bytes_received] = '\0'; 

    return bytes_received; 
}

//! Equal to the implementation in client.c
//! but not enough similarities for now to make a separate file
int is_ipv4(char *address) {
    struct in_addr ipv4_addr;
    int result = inet_pton(AF_INET, address, &ipv4_addr); // tries the conversion
    return result == 1; // 1 if it's IPv4
}

void close_service(UniSocket_t *p_server_t)
{
    // Signal all the threads to finish
    pthread_mutex_lock(&g_mutex_server);
    *(p_server_t->p_quit_signal) = CLOSE_SERVER;
    pthread_mutex_unlock(&g_mutex_server);

    // Clean information stored during the service execution
    pthread_mutex_lock(p_server_t->p_mutex_usernames_ht);
    hash_table_destroy_with_ptr_to_ptr(&p_server_t->p_usernames_ht);
    pthread_mutex_unlock(p_server_t->p_mutex_usernames_ht);

    close_server_socket(p_server_t);
    INFO_VERBOSE_LOG("Service closed.\n");
}

int join_thread_and_handle_errors(pthread_t *p_thread_ID)
{
    void *p_thread_return_val;
    int join_status;
    if ((join_status = pthread_join(*p_thread_ID, &p_thread_return_val)) < 0)
    {
        ERROR_VERBOSE_LOG("Error joining listener thread");
        return join_status;
    }
    //
    if (p_thread_return_val == NULL)
    {
        ERROR_VERBOSE_LOG("pthread_join could not allocate memory for the thread return value");
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

// TODO consider allocating memory for the expected number of clients, and reusing
// TODO instead of allocating every time
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
    //
    pthread_mutex_t *p_mutex_usernames_ht = p_server_t->p_mutex_usernames_ht;
    hash_table *p_usernames_ht = p_server_t->p_usernames_ht;
    //
    pthread_mutex_t *p_mutex_online_clients_set = p_server_t->p_mutex_online_clients_set;
    fd_set *p_online_clients_set = p_server_t->p_online_clients_set;
    //
    pthread_mutex_t *p_mutex_common_msg_buffer = p_server_t->p_mutex_common_msg_buffer;
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
                // Send the needed variables with the client struct
                p_client_t->p_mutex_quit_signal = p_mutex_quit_signal;
                p_client_t->p_quit_signal = p_quit_signal;
                //
                p_client_t->p_mutex_usernames_ht = p_mutex_usernames_ht;
                p_client_t->p_usernames_ht = p_usernames_ht;
                //
                p_client_t->p_mutex_online_clients_set = p_mutex_online_clients_set;
                p_client_t->p_online_clients_set = p_online_clients_set;
                //
                p_client_t->p_mutex_common_msg_buffer = p_mutex_common_msg_buffer;
                p_client_t->p_common_msg_buffer = p_server_t->p_common_msg_buffer; 

                // Forward the client to the desired service function
                int (*functionPtr)(ClientInfo_t *) = p_client_t->p_service_func;
                (*functionPtr)(p_client_t);

                free_client_memory_with_ptr_to_ptr((void **)&p_client_t); // in case the service did NOT
            }
        }

        // Check if the service was requested to close by other thread
        pthread_mutex_lock(p_mutex_quit_signal);
        quit_signal_val = *p_quit_signal;
        pthread_mutex_unlock(p_mutex_quit_signal);

    } while (!quit_signal_val);
}

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
            free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        }
        else
        {
            ERROR_VERBOSE_LOG("Error when dequeuing the client, found a null value when alerting the clients of closing the server");
        }
    }

    pthread_mutex_unlock(p_queue_mutex);
}

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
            ERROR_VERBOSE_LOG("Error allocating memory for the client info struct");
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

void initialize_server_concurrency_and_thread_pool(UniSocket_t *p_server_t)
{
    // Thread pool
    pthread_t *p_thread_pool = malloc(SIZE_THREAD_POOL * sizeof(pthread_t));
    if (p_thread_pool == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for thread pool");
        exit(EXIT_FAILURE);
    }
    p_server_t->p_thread_pool = p_thread_pool;

    // Conditional variables
    pthread_cond_t *p_condition_var = malloc(sizeof(pthread_cond_t));
    if (p_condition_var == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for condition variable");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(p_condition_var, NULL) != 0)
    {
        ERROR_VERBOSE_LOG("Error initializing condition variable");
        free(p_condition_var); // Free allocated memory
        exit(EXIT_FAILURE);
    }
    p_server_t->p_condition_var = p_condition_var;

    // Mutexes
    pthread_mutex_t *p_mutex_queue = malloc(sizeof(pthread_mutex_t));
    if (p_mutex_queue == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for the QUEUE mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(p_mutex_queue, NULL) != 0)
    {
        ERROR_VERBOSE_LOG("Error initializing the QUEUE mutex");
        free(p_mutex_queue);
        exit(EXIT_FAILURE);
    }
    p_server_t->p_mutex_queue = p_mutex_queue;
    //
    pthread_mutex_t *p_mutex_quit_signal = malloc(sizeof(pthread_mutex_t));
    if (p_mutex_quit_signal == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for the QUIT SIGNAL mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(p_mutex_quit_signal, NULL) != 0)
    {
        ERROR_VERBOSE_LOG("Error initializing the QUIT SIGNAL mutex");
        free(p_mutex_quit_signal);
        exit(EXIT_FAILURE);
    }
    p_server_t->p_mutex_quit_signal = p_mutex_quit_signal;
    //
    pthread_mutex_t *p_mutex_usernames_ht = malloc(sizeof(pthread_mutex_t));
    if (p_mutex_usernames_ht == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for the CLIENT'S HASH TABLE mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(p_mutex_usernames_ht, NULL) != 0)
    {
        ERROR_VERBOSE_LOG("Error initializing the CLIENT'S HASH TABLE mutex");
        free(p_mutex_usernames_ht);
        exit(EXIT_FAILURE);
    }
    p_server_t->p_mutex_usernames_ht = p_mutex_usernames_ht;
    //
    pthread_mutex_t *p_mutex_online_clients_set = malloc(sizeof(pthread_mutex_t));
    if (p_mutex_online_clients_set == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for the CLIENT'S ONLINE CLIENTS SET mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(p_mutex_online_clients_set, NULL) != 0)
    {
        ERROR_VERBOSE_LOG("Error initializing the CLIENT'S ONLINE CLIENTS SET mutex");
        free(p_mutex_online_clients_set);
        exit(EXIT_FAILURE);
    }
    p_server_t->p_mutex_online_clients_set = p_mutex_online_clients_set;
    //
    pthread_mutex_t *p_mutex_common_msg_buffer = malloc(sizeof(pthread_mutex_t));
    if (p_mutex_common_msg_buffer == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for the COMMON MSG BUFFER mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(p_mutex_common_msg_buffer, NULL) != 0)
    {
        ERROR_VERBOSE_LOG("Error initializing the COMMON MSG BUFFER mutex");
        free(p_mutex_common_msg_buffer);
        exit(EXIT_FAILURE);
    }
    p_server_t->p_mutex_common_msg_buffer = p_mutex_common_msg_buffer;

    // Quit signal (to finish threads gracefully)
    volatile sig_atomic_t *p_quit_signal = malloc(sizeof(sig_atomic_t));
    if (p_quit_signal == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for quit signal");
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
            ERROR_VERBOSE_LOG("Error creating one of the pool threads of the server");
            continue;
        }
    }
}

hash_table *get_usernames_hash_table_ptr()
{
    hash_table *p_usernames_ht;

    uint32_t size = FD_SETSIZE;
    hashFunc *p_hash_func = murmur3_32_hash;
    cleanObjFunc *p_cleanup_func = free_client_memory_with_ptr_to_ptr;
    p_usernames_ht = hash_table_create(size, p_hash_func, p_cleanup_func);

    if (p_usernames_ht == NULL)
    {
        ERROR_VERBOSE_LOG("Error creating the hash table for the usernames");
        return NULL;
    }

    return p_usernames_ht;
}

fd_set *get_set_of_clients_descriptors()
{
    fd_set *p_online_clients_set = malloc(sizeof(fd_set));
    if (p_online_clients_set == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for the set of socket descriptors for the clients");
        return NULL;
    }

    FD_ZERO(p_online_clients_set);
    return p_online_clients_set;
}

int listen_for_connections_on_separate_thread(UniSocket_t *p_server_t)
{
    pthread_t listening_thread;
    int creation_status;
    if ((creation_status = pthread_create(&listening_thread, NULL, (void *(*)(void *))accept_incoming_connections, (void *)p_server_t)) < 0)
    {
        ERROR_VERBOSE_LOG("Error creating the listening thread for the server");
        return creation_status;
    }
    //
    int join_status;
    if ((join_status = join_thread_and_handle_errors(&listening_thread)) < 0)
    {
        ERROR_VERBOSE_LOG("Error joinin listening thread of the server");
        return join_status;
    }

    return 0;
}

int start_accepting_incoming_connections(UniSocket_t *p_server_t)
{
    // For the client's structs, to allow data sharing between threads
    hash_table *p_usernames_ht = get_usernames_hash_table_ptr();
    if (p_usernames_ht == NULL)
    {
        ERROR_VERBOSE_LOG("Error, unexpected value for the pointer of the usernames table. Possible creation failure.");
        return -1;
    }
    p_server_t->p_usernames_ht = p_usernames_ht;

    // To know which clients are ONLINE
    fd_set *p_online_clients_set = get_set_of_clients_descriptors();
    if (p_online_clients_set == NULL)
    {
        ERROR_VERBOSE_LOG("Error, unexpected value for the pointer of the online clients socket descriptors set. Possible creation failure.");
        return -1;
    }
    p_server_t->p_online_clients_set = p_online_clients_set;
    //
    int *p_max_socket_so_far = (int *)malloc(sizeof(int)); // TODO use to improve perfomance when selecting active file descriptors
    p_server_t->p_max_socket_so_far = p_max_socket_so_far;

    // Prepare the buffer for the common messages 
    char *p_common_msg_buffer = (char *)malloc(BUFFER_SIZE * MAX_NUM_CLIENTS); 
    p_server_t->p_common_msg_buffer = p_common_msg_buffer;

    // Does NOT return error values because it ends the execution in that case
    // (the consequences would be too critical to handle)
    initialize_server_concurrency_and_thread_pool(p_server_t);

    int listen_exit_status;
    if ((listen_exit_status = listen_for_connections_on_separate_thread(p_server_t)) < 0)
    {
        ERROR_VERBOSE_LOG("Error creating or executing the listening thread");
        return listen_exit_status;
    }

    return 0;
}

int start_service(int server_port, char *server_ip, ServiceFunctionPtr p_service_func_arg)
{
    // Create the server socket
    UniSocket_t *p_server_t = create_socket_struct(true, server_port, is_ipv4(server_ip), server_ip);
    if (p_server_t == NULL) {
        perror("Error creating socket for the client for the desired service");
        return -1;
    }
    INFO_VERBOSE_LOG("Client socket created successfully\n");

    // Assign the service function
    p_server_t->p_service_func = p_service_func_arg;

    // Start the server
    int error_status;
    if ((error_status = start_accepting_incoming_connections(p_server_t)) < 0)
    {
        ERROR_VERBOSE_LOG("Error acepting incoming connections");
        return error_status;
    }

    close_service(p_server_t);
}
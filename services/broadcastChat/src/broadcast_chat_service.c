#include "broadcast_chat_service.h"

// Let only a thread at a time to access the main service struct
static pthread_mutex_t g_mutex_server = PTHREAD_MUTEX_INITIALIZER;


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
//TODO solve the problem of removing the username from the hash table
void wait_for_thread_work(BroadcastControllers_t *p_broadcast_ctrls_t_s) {

    bool is_broadcasting_copy = true;
    while (is_broadcasting_copy)
    {

        // Check if the broadcast service does NOT have any thread handling it
        pthread_mutex_lock(&g_mutex_server);
        if (!p_broadcast_ctrls_t_s->p_threads_t->is_broadcasting)
        {
            p_broadcast_ctrls_t_s->p_threads_t->is_broadcasting = true; // handle the needed work
            pthread_mutex_unlock(&g_mutex_server);
            is_broadcasting_copy = false;
        }
        else
        {
            pthread_cond_wait(&p_broadcast_ctrls_t_s->p_threads_t->p_broadcast_condition_var, &g_mutex_server);
        }
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int start_removing_client_from_broadcast(BroadcastControllers_t *p_broadcast_ctrls_t_s, ClientInfo_t *p_client_t, int socket_FD, fd_set ready_sockets_set) {
    if (remove_socket_from_sets(p_client_t, socket_FD, ready_sockets_set) < 0) {
        perror("Error removing socket from the ready sockets set");
        return -1;
    }

    //TODO Make the used username available for the next clients
    pthread_mutex_lock(p_client_t->p_mutex_usernames_ht);
    if (hash_table_delete_element(p_client_t->p_usernames_ht, (const char *)p_client_t->name) == NULL) {
        perror("Tried to delete, from the hash table, a username that was NOT there");
        return -1;
    }
    pthread_mutex_unlock(p_client_t->p_mutex_usernames_ht);

    // Let another thread handle the broadcast
    pthread_mutex_lock(&g_mutex_server);
    p_broadcast_ctrls_t_s->p_threads_t->is_broadcasting = false;
    pthread_cond_signal(p_broadcast_ctrls_t_s->p_threads_t->p_broadcast_condition_var);
    pthread_mutex_unlock(&g_mutex_server);

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int send_customized_welcome_message(ClientInfo_t *p_client_t)
{
    // Get the necessary data from the client
    char *p_buffer_cli = p_client_t->buffer;
    char *p_name_cli = p_client_t->name;
    int client_FD = p_client_t->sock_FD;

    memset(p_buffer_cli, 0, BUFFER_SIZE);
    char welcome_msg[BUFFER_SIZE];
    strcpy(welcome_msg, "Welcome to the broadcast channel, ");
    strcat(welcome_msg, p_name_cli);
    strcat(welcome_msg, "!\n");
    //
    strncpy(p_buffer_cli, welcome_msg, strlen(welcome_msg));
    int sending_status;
    if ((sending_status = send(client_FD, welcome_msg, strlen(welcome_msg), 0)) < 0) {
        perror("Error sending the customized welcome message");
        return sending_status;
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int remove_socket_from_sets(ClientInfo_t *p_client_t, int socket_FD, fd_set ready_sockets_set)
{
    // Get the necessary data from the client
    fd_set *p_online_clients_set = p_client_t->p_online_clients_set;
    pthread_mutex_t *p_mutex_online_clients_set = p_client_t->p_mutex_online_clients_set;

    // Remove the client socket descriptor from BOTH sets of file descriptors
    FD_CLR(socket_FD, &ready_sockets_set); // ...from the CURRENT set
    pthread_mutex_lock(p_mutex_online_clients_set);
    FD_CLR(socket_FD, p_online_clients_set); // ...from the ONLINE set
    pthread_mutex_unlock(p_mutex_online_clients_set);

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param a
 */

int broadcast_message_to_online_clients(ClientInfo_t *p_client_t, long tot_num_bytes_recv)
{

    // Get the needed data from the client
    char *p_buffer_cli = p_client_t->buffer;
    pthread_mutex_t *p_mutex_usernames_ht = p_client_t->p_mutex_usernames_ht;
    hash_table *p_usernames_ht = p_client_t->p_usernames_ht;
    char *p_common_msg_buffer = p_client_t->p_common_msg_buffer;

    p_buffer_cli[tot_num_bytes_recv - 1] = '\0';
    pthread_mutex_lock(p_mutex_usernames_ht);
    for (uint32_t i = 0; i < p_usernames_ht->size; i++)
    {
        if (p_usernames_ht->elements[i] != NULL)
        {
            // Get all the usernames present in the hash table at the moment
            entry *p_temp = p_usernames_ht->elements[i];
            while (p_temp != NULL)
            {
                // Get the client information from the hash table
                ClientInfo_t *p_target_client_t = hash_table_lookup(p_usernames_ht, (const char *)p_temp->p_key);
                send(p_target_client_t->sock_FD, p_common_msg_buffer, strlen(p_common_msg_buffer), 0);

                // Get the next user (if stored)
                p_temp = p_temp->p_next;
            }
        }
    }
    pthread_mutex_unlock(p_mutex_usernames_ht);

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */

int get_messages_from_active_clients(BroadcastControllers_t *p_broadcast_ctrls_t_s, ClientInfo_t *p_client_t)
{

    // Space for the broadcast message
    char *p_client_msg_edited = (char *)malloc(BUFFER_SIZE + MAX_NUM_CLIENTS + 3);

    if (send_customized_welcome_message(p_client_t) < 0) {
        perror("Error sending the customized message");
    }

    // Add the socket descriptor to the shared list
    pthread_mutex_lock(p_client_t->p_mutex_online_clients_set);
    FD_SET(p_client_t->sock_FD, p_client_t->p_online_clients_set);
    pthread_mutex_unlock(p_client_t->p_mutex_online_clients_set);

    // Let a thread attend only a client AT A TIME
    wait_for_thread_work(p_broadcast_ctrls_t_s); 

    // Use assync I/O to share the messages
    fd_set ready_sockets_set;
    int quit_signal_val;
    do
    {
        // Get a copy of the set of ONLINE clients
        // A mutex is NOT needed, there is only one thread handling this at a time
        ready_sockets_set = *p_client_t->p_online_clients_set;

        if (select(FD_SETSIZE, &ready_sockets_set, NULL, NULL, NULL) < 0)
        {
            perror("Error select while broadcasting clients");
            exit(EXIT_FAILURE);
        }

        // Reset possible previous messages from the buffer
        memset(p_client_t->p_common_msg_buffer, 0, BUFFER_SIZE + MAX_NUM_CLIENTS + 3);
        int tot_num_bytes_recv = 0;

        // Gather the messages to be broadcasted
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &ready_sockets_set))
            {

                // Receive the content that the user MAY have sent
                memset(p_client_t->buffer, 0, BUFFER_SIZE); // TODO avoid so much memset with strategic '\0'
                int bytes_received;
                if ((bytes_received = recv(i, p_client_t->buffer, BUFFER_SIZE, 0)) < 0)
                {
                    perror("Error receiving content from one of the clients. Maybe terminated the connection.");

                    if (start_removing_client_from_broadcast(p_broadcast_ctrls_t_s, p_client_t, i, ready_sockets_set) < 0) {
                        perror("Error when starting to remove client from the broadcast session");
                    }
                    start_removing_client_from_broadcast(p_broadcast_ctrls_t_s, p_client_t, i, ready_sockets_set);

                    // Leave the broadcast for another thread
                    // if the user with error was the one handled by this one
                    // because there will be other threads handling its rescpective newcome clients
                    if (i == p_client_t->sock_FD)
                    {
                        return 0;
                    }
                }
                else if (bytes_received == 0)
                {
                    perror("Client terminated the connection");

                    if (start_removing_client_from_broadcast(p_broadcast_ctrls_t_s, p_client_t, i, ready_sockets_set) < 0) {
                        perror("Error when starting to remove client from the broadcast session");
                    }
                    close_socket_with_ptr_if_open(&i);
                    //
                    if (i == p_client_t->sock_FD) // same reason as above
                    {
                        return 0;
                    }
                }
                else
                {
                    // Get the total amount of sent data
                    tot_num_bytes_recv += bytes_received; // TODO usar

                    // Edite the preview of the message for the client...
                    pthread_mutex_lock(p_client_t->p_mutex_common_msg_buffer);
                    sprintf(p_client_msg_edited, "%s> %s", p_broadcast_ctrls_t_s->usernames_by_FD_arr[i], p_client_t->buffer);
                    strcat(p_client_t->p_common_msg_buffer, p_client_msg_edited);
                    pthread_mutex_unlock(p_client_t->p_mutex_common_msg_buffer);
                    //
                    memset(p_client_msg_edited, 0, BUFFER_SIZE);
                }
            }
        }

        if (broadcast_message_to_online_clients(p_client_t, tot_num_bytes_recv) < 0) {
            perror("Error broadcasting the message to the online clients");
        }

        // Check if the server was requested to finish by some other thread
        pthread_mutex_lock(p_client_t->p_mutex_quit_signal);
        quit_signal_val = *p_client_t->p_quit_signal;
        pthread_mutex_unlock(p_client_t->p_mutex_quit_signal);

    } while (!quit_signal_val);

    // Remove the username from the hash table
    pthread_mutex_lock(p_client_t->p_mutex_usernames_ht);
    hash_table_delete_element(p_client_t->p_usernames_ht, (const char *)p_client_t->name);
    pthread_mutex_unlock(p_client_t->p_mutex_usernames_ht);

    return 0;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int accept_and_store_client_username(BroadcastControllers_t *p_broadcast_ctrls_t_s, ClientInfo_t *p_client_t) {
    // Assign the username to its struct (temporary memory)
    strncpy(p_client_t->name, p_client_t->buffer, strlen(p_client_t->buffer));
    p_client_t->name[MAX_USERNAME_LENGTH - 1] = '\0';

    // Inform the acceptance of the username
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    const char* accepted_msg = "\nUsername accepted!\n";
    strncpy(p_client_t->buffer, accepted_msg, strlen(accepted_msg));
    p_client_t->buffer[strlen(accepted_msg)] = '\0';
    send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0);

    // Place the username in the hash table
    pthread_mutex_lock(p_client_t->p_mutex_usernames_ht);
    hash_table_insert(p_client_t->p_usernames_ht, p_client_t->name, p_client_t);
    pthread_mutex_unlock(p_client_t->p_mutex_usernames_ht);

    // Relate the file descriptor to its username
    pthread_mutex_lock(&g_mutex_server);
    strncpy(p_broadcast_ctrls_t_s->usernames_by_FD_arr[p_client_t->sock_FD], p_client_t->name, MAX_USERNAME_LENGTH);
    pthread_mutex_unlock(&g_mutex_server);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */

int prepare_to_broadcast_chat(BroadcastControllers_t *p_broadcast_ctrls_t_s, ClientInfo_t *p_client_t)
{

    // Ask for a simple ID
    const char *init_msg = "\nOur newest guest! How can we call you? ";
    strncpy(p_client_t->buffer, init_msg, strlen(init_msg));
    send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0);

    // Get pretended username
    bool is_valid_username = false;
    while (is_valid_username == false)
    {

        // Receive the pretended username
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
        int bytes_received;
        if ((bytes_received = recv(p_client_t->sock_FD, p_client_t->buffer, BUFFER_SIZE, 0)) < 0)
        {
            perror("Error receiving the preferred name from the client");
            exit(EXIT_FAILURE);
        }
        else if (bytes_received == 0)
        {
            printf("Client terminated the connection.\n");
            exit(EXIT_SUCCESS);
        }

        // Sanitize the input (and allow for a correct comparison of names)
        if (bytes_received <= BUFFER_SIZE)
        {
            p_client_t->buffer[bytes_received - 1] = '\0';
        }

        // Check if present in the session
        pthread_mutex_lock(p_client_t->p_mutex_usernames_ht);
        void *p_lookup_result = hash_table_lookup(p_client_t->p_usernames_ht, (const char *)p_client_t->buffer);
        pthread_mutex_unlock(p_client_t->p_mutex_usernames_ht);

        if (p_lookup_result == NULL) // did NOT find that element
        {
            accept_and_store_client_username(p_broadcast_ctrls_t_s, p_client_t);
            is_valid_username = true;
        }
        else
        { // Report invalid username
            memset(p_client_t->buffer, 0, BUFFER_SIZE);
            const char *rejection_msg = "\nThat username is present in the session...\nPlease, choose another: ";
            strncpy(p_client_t->buffer, rejection_msg, strlen(rejection_msg));
            send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0);
        }
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */

void *prepare_client_structs_for_data(ClientInfo_t *p_client_t)
{
    if (p_client_t == NULL)
    {
        perror("Received a pointer pointing to improperly allocated memory");
        return NULL;
    }
    //
    p_client_t->name = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (p_client_t->name == NULL)
    {
        perror("Error alocating memory for the p_client_t struct pointer");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return NULL;
    }

    p_client_t->buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (p_client_t->buffer == NULL)
    {
        perror("Error alocating memory for the buffer pointer");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return NULL;
    }

    // Get the client's IP address (for a unique identification)
    char ip_buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, (void *)&(p_client_t->p_addr), ip_buffer, INET_ADDRSTRLEN) == NULL) // TODO handle this error, and IPV6
    {
        perror("Error converting IP address");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return NULL;
    }
    //
    p_client_t->addr_info = ip_buffer;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
ThreadControl_t *get_thread_control_struct_ptr()
{
    ThreadControl_t *p_threads_t = (ThreadControl_t *)malloc(sizeof(ThreadControl_t));
    if (p_threads_t == NULL)
    {
        perror("Error allocating memory for the thread control struct");
        return NULL;
    }

    // Allocate
    p_threads_t->p_broadcast_condition_var = (pthread_cond_t *)malloc(sizeof(pthread_mutex_t));
    if (p_threads_t->p_broadcast_condition_var == NULL)
    {
        perror("Error allocating memory for mutex");
        return NULL;
    }
    //
    p_threads_t->is_broadcasting = (bool *)malloc(sizeof(bool));
    if (p_threads_t->is_broadcasting == NULL)
    {
        perror("Error allocating memory for the working bool indicator");
        return NULL;
    }
    //
    p_threads_t->did_prepare_service_data = (bool *)malloc(sizeof(bool));
    if (p_threads_t->did_prepare_service_data == NULL)
    {
        perror("Error allocating memory for the service data boolean value");
        return NULL;
    }

    // Initialiaze
    pthread_cond_init(p_threads_t->p_broadcast_condition_var, NULL);
    p_threads_t->is_broadcasting = false;
    p_threads_t->did_prepare_service_data = false;

    return p_threads_t;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
BroadcastControllers_t *get_broadcast_controls_struct_ptr()
{
    BroadcastControllers_t *p_broadcast_ctrls_t_s = (BroadcastControllers_t *)malloc(sizeof(BroadcastControllers_t));
    if (p_broadcast_ctrls_t_s == NULL)
    {
        perror("Error alocating memory for the main server's controls struct");
        return NULL;
    }

    // Prepare the array of usernames (avoids multiple lookups on the hash table)
    char **usernames_by_FD_arr = malloc(FD_SETSIZE * sizeof(char *));
    if (usernames_by_FD_arr == NULL)
    {
        perror("Error, unexpected value for the array of usernames indexes by file descriptors. Possible creation failure.");
        return NULL;
    }
    //
    for (int i = 0; i < FD_SETSIZE; i++)
    {
        usernames_by_FD_arr[i] = (char *)malloc(MAX_USERNAME_LENGTH);
        if (usernames_by_FD_arr[i] == NULL)
        {
            perror("Error, unexpected value for a username string. Possible creation failure.");
            return NULL;
        }
    }
    p_broadcast_ctrls_t_s->usernames_by_FD_arr = usernames_by_FD_arr;

    // Compact the threads control variables onto a structure
    ThreadControl_t *p_threads_t;
    if ((p_threads_t = get_thread_control_struct_ptr()) == NULL)
    {
        perror("Error getting the thread control struct");
        return NULL;
    }
    p_broadcast_ctrls_t_s->p_threads_t = p_threads_t;

    return p_broadcast_ctrls_t_s;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */

int prepare_client_for_broadcast_and_start(ClientInfo_t *p_client_t)
{
    static bool did_first_allocation = false;

    // Start the server if NOT started before
    static BroadcastControllers_t *p_broadcast_ctrls_t_s;
    pthread_mutex_lock(&g_mutex_server);
    if (!did_first_allocation)
    {

        if ((p_broadcast_ctrls_t_s = get_broadcast_controls_struct_ptr()) == NULL)
        {
            perror("Error starting the service");
            return -1;
        }

        did_first_allocation = true;
    }
    pthread_mutex_unlock(&g_mutex_server);

    if ((prepare_client_structs_for_data(p_client_t)) == NULL)
    {
        perror("Error preparing client structs for the data");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return -1;
    }
    //
    int exit_status;
    if ((exit_status = prepare_to_broadcast_chat(p_broadcast_ctrls_t_s, p_client_t)) < 0)
    {
        perror("Error preparing client to be broadcasted");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return exit_status;
    }
    //
    if ((exit_status = get_messages_from_active_clients(p_broadcast_ctrls_t_s, p_client_t)) < 0)
    {
        perror("Error when broadcasting the client");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return exit_status;
    }

    free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
}
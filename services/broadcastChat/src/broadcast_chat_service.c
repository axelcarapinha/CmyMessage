#include "broadcast_chat_service.h"

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Sends a customized welcome message to a client.
 *
 * This function retrieves necessary data from the client, constructs a welcome
 * message including the client's name, and sends it to the client.
 *
 * @param p_client_t Pointer to the structure containing client information.
 */
void send_customized_welcome_message(ClientInfo_t *p_client_t)
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
    strcpy(p_buffer_cli, welcome_msg);
    send(client_FD, welcome_msg, strlen(welcome_msg), 0);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Removes a socket descriptor from sets of file descriptors.
 *
 * This function removes a specified socket descriptor from both the set of
 * ready sockets and the set of online clients' sockets.
 *
 * @param p_client_t Pointer to the structure containing client information.
 * @param socket_FD The socket descriptor to be removed.
 * @param ready_sockets_set The set of ready sockets.
 */
void remove_socket_from_sets(ClientInfo_t *p_client_t, int socket_FD, fd_set ready_sockets_set)
{
    // Get the necessary data from the client
    fd_set *p_online_clients_set = p_client_t->p_online_clients_set;
    pthread_mutex_t *p_mutex_online_clients_set = p_client_t->p_mutex_online_clients_set;

    // Remove the client socket descriptor from BOTH sets of file descriptors
    FD_CLR(socket_FD, &ready_sockets_set); // ...from the CURRENT set
    pthread_mutex_lock(p_mutex_online_clients_set);
    FD_CLR(socket_FD, p_online_clients_set); // ...from the ONLINE set
    pthread_mutex_unlock(p_mutex_online_clients_set);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param a
 */

void broadcast_message_to_online_clients(ClientInfo_t *p_client_t, long tot_num_bytes_recv)
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
                // TODO array with messages, and sent more customised layout ([PAGGInG] message <username)
                // TODO have a pointer for the array

                // Get the client information from the hash table
                ClientInfo_t *p_target_client_t = hash_table_lookup(p_usernames_ht, (const char *)p_temp->p_key);
                send(p_target_client_t->sock_FD, p_common_msg_buffer, strlen(p_common_msg_buffer), 0);

                // Get the next user (if stored)
                p_temp = p_temp->p_next;
            }
        }
    }
    pthread_mutex_unlock(p_mutex_usernames_ht);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param a
 */

// TODO consider a variable to allow for verificatino from all the threads
int get_messages_from_active_clients(ClientInfo_t *p_client_t)
{
    // Organize information
    char *p_name_cli = p_client_t->name;
    char *p_buffer_cli = p_client_t->buffer;
    int client_FD = p_client_t->sock_FD;

    // Organize thread variables
    pthread_mutex_t *p_mutex_usernames_ht = p_client_t->p_mutex_usernames_ht;
    pthread_mutex_t *p_mutex_online_clients_set = p_client_t->p_mutex_online_clients_set;
    pthread_mutex_t *p_mutex_quit_signal = p_client_t->p_mutex_quit_signal;
    //
    fd_set *p_online_clients_set = p_client_t->p_online_clients_set;
    hash_table *p_usernames_ht = p_client_t->p_usernames_ht;
    volatile sig_atomic_t *p_quit_signal = p_client_t->p_quit_signal;

    // Space for the broadcast message
    char *p_client_msg_edited = (char *)malloc(BUFFER_SIZE + MAX_NUM_CLIENTS + 3);
    pthread_mutex_t *p_mutex_common_msg_buffer = p_client_t->p_mutex_common_msg_buffer;
    char *p_common_msg_buffer = p_client_t->p_common_msg_buffer;

    send_customized_welcome_message(p_client_t);

    // Add the socket descriptor to the shared list
    pthread_mutex_lock(p_mutex_online_clients_set);
    FD_SET(p_client_t->sock_FD, p_online_clients_set);
    pthread_mutex_unlock(p_mutex_online_clients_set);

    // Use assync I/O to share the messages
    fd_set ready_sockets_set;
    int quit_signal_val;
    do
    {
        // Get a copy of the set of ONLINE clients
        pthread_mutex_lock(p_mutex_online_clients_set);
        ready_sockets_set = *p_online_clients_set;
        pthread_mutex_unlock(p_mutex_online_clients_set);
        if (select(FD_SETSIZE, &ready_sockets_set, NULL, NULL, NULL) < 0) // TODO mudei number of clients em vez de FD_SETSIZE
        {
            perror("Error select while broadcasting clients");
            exit(EXIT_FAILURE);
        }

        // Reset possible previous messages from the buffer
        memset(p_common_msg_buffer, 0, BUFFER_SIZE + MAX_NUM_CLIENTS + 3);
        int tot_num_bytes_recv = 0;

        // Gather the messages to be broadcasted
        for (int i = 0; i < FD_SETSIZE; i++) // TODO mudar o setsize
        {
            if (FD_ISSET(i, &ready_sockets_set))
            {
                // Get the associated client struct //TODO make more efficient
                // pthread_mutex_lock(p_mutex_usernames_ht);
                // for (uint32_t i = 0; i < p_usernames_ht->size; i++)
                // {
                //     if (p_usernames_ht->elements[i] != NULL)
                //     {
                //         // Get all the usernames present in the hash table at the moment
                //         entry *p_temp = p_usernames_ht->elements[i];
                //         while (p_temp != NULL)
                //         {

                //             // Get the client information from the hash table
                //             ClientInfo_t *p_target_client_t = hash_table_lookup(p_usernames_ht, (const char *)p_temp->p_key);
                //             if (i == p_target_client_t->sock_FD) {

                //             }

                //             // Get the next user (if stored)
                //             p_temp = p_temp->p_next;
                //         }
                //     }
                // }
                // pthread_mutex_unlock(p_mutex_usernames_ht);

                // Receive the content that the user MAY have sent
                memset(p_buffer_cli, 0, BUFFER_SIZE); // TODO avoid so much memset with strategic '\0'
                int bytes_received;
                if ((bytes_received = recv(i, p_buffer_cli, BUFFER_SIZE, 0)) < 0)
                {
                    perror("Error receiving content from one of the clients. Maybe terminated the connection.");

                    remove_socket_from_sets(p_client_t, i, ready_sockets_set);

                    // // Make the used username available for the next clients
                    // pthread_mutex_lock(p_mutex_usernames_ht);
                    // hash_table_delete_element(p_usernames_ht, (const char *)p_client_t->name);
                    // pthread_mutex_unlock(p_mutex_usernames_ht);

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

                    remove_socket_from_sets(p_client_t, i, ready_sockets_set);

                    // Make the used username available for the next clients
                    // TODO find the user with this socket, and remove its username
                    // pthread_mutex_lock(p_mutex_usernames_ht);
                    // hash_table_delete_element(p_usernames_ht, (const char *)p_client_t->name);
                    // pthread_mutex_unlock(p_mutex_usernames_ht);

                    if (i == p_client_t->sock_FD) // same reason as above
                    {
                        return 0;
                    }

                    close_socket_with_ptr_if_open(&i);
                }
                else
                {
                    // Get the total amount of sent data
                    tot_num_bytes_recv += bytes_received; // TODO usar

                    // Edite the preview of the message for the client...
                    pthread_mutex_lock(p_mutex_common_msg_buffer);
                    sprintf(p_client_msg_edited, "User %d> %s", i, p_buffer_cli);
                    strcat(p_common_msg_buffer, p_client_msg_edited); // TODO change the strcat
                    pthread_mutex_unlock(p_mutex_common_msg_buffer);
                    //
                    memset(p_client_msg_edited, 0, BUFFER_SIZE);
                }
            }
        }

        broadcast_message_to_online_clients(p_client_t, tot_num_bytes_recv);

        // Check if the server was request to finish by some thread
        pthread_mutex_lock(p_mutex_quit_signal);
        quit_signal_val = *p_quit_signal;
        pthread_mutex_unlock(p_mutex_quit_signal);

    } while (!quit_signal_val);

    // Remove the username from the hash table
    pthread_mutex_lock(p_mutex_usernames_ht);
    hash_table_delete_element(p_usernames_ht, (const char *)p_client_t->name);
    pthread_mutex_unlock(p_mutex_usernames_ht);

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Prepares a client to broadcast chat messages
 *
 * This function prepares a client to broadcast chat messages by interacting with the client.
 * It asks the client for their name, receives the name, and sends a customized welcome message.
 * If any error occurs during interaction with the client, it prints an error message
 * and terminates the program with the appropriate exit status.
 *
 * @param p_client_t Pointer to the ClientInfo_t structure representing the client
 *
 * @return 0 on success
 */

int prepare_to_broadcast_chat(ClientInfo_t *p_client_t)
{
    // Prepare the pointers for the necessary data (for a cleaner code)
    char *p_name_cli = p_client_t->name;
    char *p_buffer_cli = p_client_t->buffer;
    int client_FD = p_client_t->sock_FD;
    //
    hash_table *p_usernames_ht = p_client_t->p_usernames_ht;
    pthread_mutex_t *p_mutex_usernames_ht = p_client_t->p_mutex_usernames_ht;

    // Ask for a simple ID
    strcpy(p_buffer_cli, "\nOur newest guest! How can we call you? ");
    send(client_FD, p_buffer_cli, strlen(p_buffer_cli), 0);

    // Get pretended username
    bool is_valid_username = false;
    while (is_valid_username == false)
    {

        // Receive the pretended username
        memset(p_buffer_cli, 0, BUFFER_SIZE);
        int bytes_received;
        if ((bytes_received = recv(client_FD, p_buffer_cli, BUFFER_SIZE, 0)) < 0)
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
            p_buffer_cli[bytes_received - 1] = '\0';
        }

        // Check if present in the session
        pthread_mutex_lock(p_mutex_usernames_ht);
        void *p_lookup_result = hash_table_lookup(p_usernames_ht, (const char *)p_buffer_cli);
        pthread_mutex_unlock(p_mutex_usernames_ht);

        if (p_lookup_result == NULL)
        {

            // Assign the username to its struct (temporary memory)
            strcpy(p_name_cli, p_buffer_cli);

            // Inform the acceptance of the username
            memset(p_buffer_cli, 0, BUFFER_SIZE);
            strcpy(p_buffer_cli, "\nUsername accepted!\n");
            send(client_FD, p_buffer_cli, strlen(p_buffer_cli), 0);

            // Place the username in the hash table
            pthread_mutex_lock(p_mutex_usernames_ht);
            hash_table_insert(p_usernames_ht, p_name_cli, p_client_t);
            pthread_mutex_unlock(p_mutex_usernames_ht);

            is_valid_username = true;
        }
        else
        { // Report invalid username
            memset(p_buffer_cli, 0, BUFFER_SIZE);
            strcpy(p_buffer_cli, "\nThat username is present in the session...\nPlease, chose another: ");
            send(client_FD, p_buffer_cli, strlen(p_buffer_cli), 0);
        }
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Prepares client structures for data
 *
 * This function prepares client structures for storing data.
 * It allocates memory for the client's name and buffer, and retrieves the client's IP address
 * for unique identification. If any error occurs during memory allocation or IP address retrieval,
 * it prints an error message, frees the client's memory, and returns NULL.
 *
 * @param p_client_t Pointer to the ClientInfo_t structure representing the client
 *
 * @return Pointer to the prepared client structure on success, or NULL if an error occurs
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
    if (inet_ntop(AF_INET, (void *)&(p_client_t->p_addr), ip_buffer, INET_ADDRSTRLEN) == NULL) // TODO handle this error
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
 * @brief Prepares a client for broadcast and starts broadcasting
 *
 * This function prepares a client for broadcast by initializing necessary data structures
 * and then starts broadcasting the client's data. It first prepares the client's data structures
 * for broadcasting, then prepares the client for broadcasting, and finally broadcasts the client.
 * If any error occurs during preparation or broadcasting, it prints an error message,
 * frees the client's memory, and returns the corresponding error status.
 *
 * @param p_client_t Pointer to the ClientInfo_t structure representing the client
 *
 * @return 0 on success, or a negative value indicating an error
 */

int prepare_client_for_broadcast_and_start(ClientInfo_t *p_client_t)
{
    if ((prepare_client_structs_for_data(p_client_t)) == NULL)
    {
        perror("Error preparing client structs for the data");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return -1;
    }
    //
    int exit_status;
    if ((exit_status = prepare_to_broadcast_chat(p_client_t)) < 0)
    {
        perror("Error preparing client to be broadcasted");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return exit_status;
    }
    //
    if ((exit_status = get_messages_from_active_clients(p_client_t)) < 0)
    {
        perror("Error when broadcasting the client");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return exit_status;
    }

    free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
}
#include "broadcast_chat_service.h"

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param a
 */
int broadcast_client(ClientInfo_t *p_client_t)
{
    // Get the client needed memory variables
    char *p_name_cli = p_client_t->name;
    char *p_buffer_cli = p_client_t->buffer;
    long client_FD = p_client_t->sock_FD;

    memset(p_buffer_cli, 0, BUFFER_SIZE);

    while (true)
    {
        size_t bytes_received;
        if ((bytes_received = recv(client_FD, p_buffer_cli, BUFFER_SIZE, 0)) > 0)
        {
            send(client_FD, p_buffer_cli, BUFFER_SIZE, 0);
            memset(p_buffer_cli, 0, BUFFER_SIZE);
        }
        else if (bytes_received == 0)
        {
            printf("The client '%s' terminated the connection\n", p_client_t->name);
            break;
        }
        else
        {
            fprintf(stderr, "Failed to receive content from the client\n");
            break;
        }
    }

    memset(p_buffer_cli, 0, BUFFER_SIZE);

    return 0;
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param a
 */
int prepare_to_broadcast_chat(ClientInfo_t *p_client_t)
{
    // Prepare the pointers for the necessary data (for a cleaner code)
    char *p_name_cli = p_client_t->name;
    char *p_buffer_cli = p_client_t->buffer;
    long client_FD = p_client_t->sock_FD;

    // Ask for a simple ID
    strcpy(p_buffer_cli, "\nOur newest guest! How we can call you: ");
    send(client_FD, p_buffer_cli, strlen(p_buffer_cli), 0);

    // Assign its ID
    memset(p_buffer_cli, 0, BUFFER_SIZE);
    int bytes_received;
    if ((bytes_received = recv(client_FD, p_buffer_cli, BUFFER_SIZE, 0)) < 0)
    {
        fprintf(stderr, "recv() function failed, with exit code %d\n", bytes_received);
        exit(EXIT_FAILURE);
    }
    else if (bytes_received == 0)
    {
        fprintf(stderr, "Client terminated the connection.\n");
        exit(EXIT_SUCCESS);
    }

    // Assign the username to its struct (temporary memory)
    if (bytes_received <= BUFFER_SIZE)
    {
        p_buffer_cli[bytes_received - 1] = '\0';
    }
    strcpy(p_name_cli, p_buffer_cli);

    // Send a customised welcome message
    char message[BUFFER_SIZE];
    strcpy(message, "Welcome to the broadcast channel, ");
    strcat(message, p_name_cli);
    strcat(message, "!\n");
    send(client_FD, message, strlen(message), 0);

    return 0;
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param a
 */
void * prepare_client_structs_for_data(ClientInfo_t *p_client_t)
{
    if (p_client_t == NULL)
    {
        perror("Received a pointer pointing to improperly allocated memory");
        return -1;
    }
    //
    p_client_t->name = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (p_client_t->name == NULL)
    {
        perror("Error alocating memory for the p_client_t struct pointer");
        free_client_memory(p_client_t);
        return NULL;
    }
    p_client_t->buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (p_client_t->buffer == NULL)
    {
        perror("Error alocating memory for the buffer pointer");
        free_client_memory(p_client_t);
        return NULL;
    }

    // Get the client's IP address (for a unique identification)
    char ip_buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(p_client_t->p_addr->s_addr), ip_buffer, INET_ADDRSTRLEN) == NULL) //TODO handle this error
    {
        perror("Error converting IP address");
        free_client_memory(p_client_t);
        exit(EXIT_FAILURE); //TODO consider changing this
    }
    //
    p_client_t->addr_info = ip_buffer;

    return 0;
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param a
 */
int prepare_client_for_broadcast_and_start(ClientInfo_t *p_client_t)
{
    int exit_status;
    //
    if ((exit_status = prepare_client_structs_for_data(p_client_t)) < 0) {
        perror("Error preparing client structs for the data");
        free_client_memory(p_client_t);
        return exit_status;
    }
    //
    if ((exit_status = prepare_to_broadcast_chat(p_client_t)) < 0) {
        perror("Error preparing client to be broadcasted");
        free_client_memory(p_client_t);
        return exit_status;
    }
    //
    if ((exit_status = broadcast_client(p_client_t)) < 0) {
        perror("Error when broadcasting the client");
        free_client_memory(p_client_t);
        return exit_status;
    }
    
    free_client_memory(p_client_t);
}
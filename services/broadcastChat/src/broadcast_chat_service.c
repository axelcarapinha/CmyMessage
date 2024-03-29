#include "broadcast_chat_service.h"

void free_client_allocated_mem(ClientInfo *client_struct_ptr)
{
    free(client_struct_ptr->name);
    memset(client_struct_ptr->buffer, 0, DEFAULT_BUFFER_SIZE);
    free(client_struct_ptr->buffer);

   // The client's memory IS freed by the sender, as it IS allocated
}

void join_client_to_broadcast_chat(ClientInfo *client_struct_ptr)
{
    // Get the client needed memory variables
    char *p_name_cli = client_struct_ptr->name;
    char *p_buffer_cli = client_struct_ptr->buffer;
    long client_FD = client_struct_ptr->client_FD;

    memset(p_buffer_cli, 0, DEFAULT_BUFFER_SIZE);

    while (true)
    {
        size_t bytes_received;
        if ((bytes_received = recv(client_FD, p_buffer_cli, DEFAULT_BUFFER_SIZE, 0)) > 0)
        {
            send(client_FD, p_buffer_cli, DEFAULT_BUFFER_SIZE, 0);
            memset(p_buffer_cli, 0, DEFAULT_BUFFER_SIZE);
        }
        else if (bytes_received == 0)
        {
            printf("The client '%s' terminated the connection\n", client_struct_ptr->name);
            break;
        }
        else
        {
            fprintf(stderr, "Failed to receive content from the client\n");
            break;
        }
    }

    memset(p_buffer_cli, 0, DEFAULT_BUFFER_SIZE);
}

void prepare_to_join_client_to_broadcast_chat(ClientInfo *client_struct_ptr)
{
    // Prepare the pointers for the necessary data (for a cleaner code)
    char *p_name_cli = client_struct_ptr->name;
    char *p_buffer_cli = client_struct_ptr->buffer;
    long client_FD = client_struct_ptr->client_FD;

    // Ask for a simple ID
    strcpy(p_buffer_cli, "\nOur newest guest! How we can call you: ");
    send(client_FD, p_buffer_cli, strlen(p_buffer_cli), 0);

    // Assign its ID
    memset(p_buffer_cli, 0, DEFAULT_BUFFER_SIZE);
    int bytes_received;
    if ((bytes_received = recv(client_FD, p_buffer_cli, DEFAULT_BUFFER_SIZE, 0)) < 0)
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
    if (bytes_received <= DEFAULT_BUFFER_SIZE)
    {
        p_buffer_cli[bytes_received - 1] = '\0';
    }
    strcpy(p_name_cli, p_buffer_cli);

    // Send a customised welcome message
    char message[DEFAULT_BUFFER_SIZE];
    strcpy(message, "Welcome to the broadcast channel, ");
    strcat(message, p_name_cli);
    strcat(message, "!\n");
    send(client_FD, message, strlen(message), 0);
}

void prepare_client_structs_for_data(ClientInfo *client_struct_ptr)
{

    // Prepare the structs for its basic information
    if (client_struct_ptr == NULL)
    {
        fprintf(stderr, "Received a pointer pointing to improperly allocated memory.");
        exit(EXIT_FAILURE);
    }
    //
    client_struct_ptr->name = (char *)calloc(DEFAULT_BUFFER_SIZE, sizeof(char));
    if (client_struct_ptr->name == NULL)
    {
        fprintf(stderr, "Error alocating memory for the client_struct_ptr struct pointer");
        exit(EXIT_FAILURE);
    }
    client_struct_ptr->buffer = (char *)calloc(DEFAULT_BUFFER_SIZE, sizeof(char));
    if (client_struct_ptr->buffer == NULL)
    {
        fprintf(stderr, "Error alocating memory for the buffer pointer");
        exit(EXIT_FAILURE);
    }

    // Get the client's IP address (for a unique identification)
    char ip_buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(client_struct_ptr->client_addr_ptr->sin_addr), ip_buffer, INET_ADDRSTRLEN) == NULL)
    {
        perror("Error converting IP address");
        close(client_struct_ptr->client_FD);
        free(client_struct_ptr->client_addr_ptr);
        free(client_struct_ptr->client_addr_len_ptr);
        free(client_struct_ptr);
        exit(EXIT_FAILURE);
    }
    //
    client_struct_ptr->addr_info = ip_buffer;
}

void prepare_client_for_broadcast_and_start(ClientInfo *client_struct_ptr)
{
    prepare_client_structs_for_data(client_struct_ptr);
    prepare_to_join_client_to_broadcast_chat(client_struct_ptr);
    join_client_to_broadcast_chat(client_struct_ptr);
    free_client_allocated_mem(client_struct_ptr);
}
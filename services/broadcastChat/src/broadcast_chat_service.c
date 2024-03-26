#include "broadcast_chat_service.h"

void join_client_to_broadcast_chat(long client_handler_FD, char *buffer, ClientInfo *client_struct_ptr)
{
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);

    while (true)
    {
        size_t bytes_received;
        if ((bytes_received = recv(client_handler_FD, buffer, DEFAULT_BUFFER_SIZE, 0)) > 0)
        {
            send(client_handler_FD, buffer, DEFAULT_BUFFER_SIZE, 0);
            memset(buffer, 0, DEFAULT_BUFFER_SIZE);
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

    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
}

void prepare_to_join_client_to_broadcast_chat(ClientInfo *client_struct_ptr)
{
    // Prepare the pointers for the necessary data (for a cleaner code)
    char *name_cli = client_struct_ptr->name;
    char *buffer_cli = client_struct_ptr->buffer;
    long client_handler_FD = client_struct_ptr->client_handler_FD;

    // Ask for a simple ID
    strcpy(buffer_cli, "\nOur newest guest! How we can call you: ");
    send(client_handler_FD, buffer_cli, strlen(buffer_cli), 0);

    // Assign its ID
    memset(buffer_cli, 0, DEFAULT_BUFFER_SIZE);
    int bytes_received;
    if ((bytes_received = recv(client_handler_FD, buffer_cli, DEFAULT_BUFFER_SIZE, 0)) < 0)
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
        buffer_cli[bytes_received - 1] = '\0';
    }
    strcpy(name_cli, buffer_cli);

    // Send a customised welcome message
    char message[DEFAULT_BUFFER_SIZE];
    strcpy(message, "Welcome to the broadcast channel, ");
    strcat(message, name_cli);
    strcat(message, "!\n");
    send(client_handler_FD, message, strlen(message), 0);

    // Join the client to the chat
    join_client_to_broadcast_chat(client_handler_FD, buffer_cli, client_struct_ptr);
}

void prepare_client_structs_for_data_and_start_joining(void *client_struct_ptr_arg)
{

    // Prepary the structs for its basic information
    ClientInfo *client_struct_ptr = (ClientInfo *)client_struct_ptr_arg;
    if (client_struct_ptr == NULL)
    {
        fprintf(stderr, "Received a pointer with baddly allocated memory");
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
        close(client_struct_ptr->client_handler_FD);
        free(client_struct_ptr->client_addr_ptr);
        free(client_struct_ptr->client_addr_len_ptr);
        free(client_struct_ptr);
        exit(EXIT_FAILURE);
    }
    //
    client_struct_ptr->addr_info = ip_buffer;

    prepare_to_join_client_to_broadcast_chat(client_struct_ptr);

    // Free the allocated memory
    free(client_struct_ptr->name);
    memset(client_struct_ptr->buffer, 0, DEFAULT_BUFFER_SIZE);
    free(client_struct_ptr->buffer);
    free(client_struct_ptr);
}

void start_broadcasting_client_on_separate_thread(void *client_struct_ptr)
{
    pthread_t cli_data_thread;
    int thread_creation_status = pthread_create(&cli_data_thread, NULL,
                                                (void *(*)(void *))prepare_client_structs_for_data_and_start_joining,
                                                (void *)client_struct_ptr);
    //
    handle_thread_creation_and_exit(thread_creation_status);
}

#include "broadcast_chat_service.h"

void join_client_to_broadcast_chat(long *client_handler_ptr, char *buffer) {
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);

    while (true) {
        recv(*client_handler_ptr, buffer, DEFAULT_BUFFER_SIZE, 0);
        send(*client_handler_ptr, buffer, DEFAULT_BUFFER_SIZE, 0);
        memset(buffer, 0, DEFAULT_BUFFER_SIZE);
    }

    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
}

void prepare_to_join_client_to_broadcast_chat(void *client_handler_ptr_arg)
{
    long *client_handler_ptr = (long *)client_handler_ptr_arg;


    char *buffer = (char *)calloc(DEFAULT_BUFFER_SIZE, sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Error alocating memory for the buffer pointer");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
    //
    ClientInfo *recipient_ptr = (ClientInfo *)malloc(sizeof(ClientInfo));
    if (recipient_ptr == NULL) {
        fprintf(stderr, "Error alocating memory for the recipient_ptr struct pointer");
        exit(EXIT_FAILURE);
    }
    recipient_ptr->name = (char *)calloc(DEFAULT_BUFFER_SIZE, sizeof(char));
    if (recipient_ptr->name == NULL) {
        fprintf(stderr, "Error alocating memory for the recipient_ptr struct pointer");
        exit(EXIT_FAILURE);
    }

    // Ask for a simple ID
    strcpy(buffer, "\nOur newest guest! How we can call you: ");
    send(*client_handler_ptr, buffer, strlen(buffer), 0);

    // Assign its ID
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
    int bytes_received;
    if ((bytes_received = recv(*client_handler_ptr, buffer, DEFAULT_BUFFER_SIZE, 0)) < 0)
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
    if (bytes_received <= DEFAULT_BUFFER_SIZE) {
        buffer[bytes_received - 1] = '\0';
    }
    strcpy(recipient_ptr->name, buffer);

    // Send a customised welcome message
    char message [DEFAULT_BUFFER_SIZE];
    strcpy(message, "Welcome to the broadcast channel, ");
    strcat(message, recipient_ptr->name);
    strcat(message, "!\n");
    send(*client_handler_ptr, message, strlen(message), 0);
    
    // Join the client to the chat
    join_client_to_broadcast_chat(client_handler_ptr, buffer);

    // Free the allocated memory
    free(recipient_ptr->name);
    free(recipient_ptr);
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
    free(buffer);
}
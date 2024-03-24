#include "broadcast_chat_service.h"

void join_client_to_broadcast_chat(long *client_handler_ptr, char *buffer)
{
    ClientInfo *recipient = (ClientInfo *)malloc(sizeof(ClientInfo));
    memset(buffer, 0, BUFFER_SIZE);

    // Ask for a simple ID
    strcpy(buffer, "\nOur newest guest! How we can call you: ");
    send(*client_handler_ptr, buffer, strlen(buffer), 0);

    // Assign its ID
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received;
    if ((bytes_received = recv(*client_handler_ptr, buffer, BUFFER_SIZE, 0)) < 0)
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
    buffer[bytes_received] = '\0';
    strcpy(recipient->name, buffer);

    char *message = "Welcome to the broadcast channel, ";
    strcat(message, buffer);
    send(*client_handler_ptr, message, strlen(message), 0);
    while (true)
    {
        // Ask for input
        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, YELLOW "> " RESET);
        send(*client_handler_ptr, buffer, strlen(buffer), 0);
    }

    // Free allocated memory
    free(recipient);
}
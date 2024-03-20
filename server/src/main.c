#include "net_utils_tcp.h"

#define PORT 8040
#define MAX_NUM_CLIENTS 30

typedef struct
{
    int socket_cli;
    struct sockaddr_in client_addr;
    char *name;
    char *recipient;
} ClientInfo;


//TODO 
void chat()
{
}


//TODO use threads
void handle_client_requests(int client_handler)
{
    ClientInfo *recipient = (ClientInfo *)malloc(sizeof(ClientInfo));

    // Ask the user for the target recipient
    char buffer[BUFFER_SIZE];
    strcpy(buffer, "Connection established!");
    strcat(buffer, "\n Recipient: ");
    size_t message_length = strlen(buffer);
    //
    if (send(client_handler, buffer, BUFFER_SIZE, 0) < message_length)
    {
        perror("Error sending the data");
    }
    else
    {
        puts("Sent a new message");
    }

    // Search for that user
    while (true)
    {
        if (recv(client_handler, buffer, BUFFER_SIZE, 0) > 0)
        {
            buffer[strlen(buffer)] = '\0';
            printf("%s\n", buffer);
        }
    }
}

void start_server()
{
    uniSocket *server_struct_ptr = create_socket(true, PORT, true);

    char buffer[BUFFER_SIZE];
    while (strcmp(fgets(buffer, BUFFER_SIZE, stdin), "finish") != 0)
    {
        int client_handler = acceptConnection(server_struct_ptr->sock_fd,
                                              server_struct_ptr->address.addr_ipv4,
                                              &(server_struct_ptr->addrlen));

        if (client_handler >= 0)
        {
            handle_client_requests(client_handler);
        }
        else
        {
            perror("Error connecting to the client");
            exit(EXIT_FAILURE);
        }
    }

    void close_socket(server_struct_ptr);
    shutdown(server_struct_ptr->sock_fd, SHUT_RDWR); //TODO implement in the close server function
    puts("Closed");
}

void main()
{
    puts("Powering up the server!");
    start_server();

    return 0;
}
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

// TODO
void chat()
{
}

// TODO use threads

void read_client_responses(int client_handler)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        recv(client_handler, buffer, BUFFER_SIZE, 0);
        // buffer[strlen(buffer) - 1] = '\0';
        send(client_handler, buffer, strlen(buffer), 0);
        printf(YELLOW "Sent: %s\n" RESET, buffer); 

        // size_t bytes_sent;
        // send(client_handler, buffer, BUFFER_SIZE, 0);   
    }

    //  while (strcmp(fgets(buffer, BUFFER_SIZE, stdin), "finish") != 0)
    // {
    // }
}

void handle_client_requests(void *client_handler_ptr)
{
    long client_handler = *((long *)client_handler);
    ClientInfo *recipient = (ClientInfo *)malloc(sizeof(ClientInfo));

    // TODO Ask for the pretended recipient
    char buffer[BUFFER_SIZE];
    strcpy(buffer, "Connection established! Press 'exit' to terminate the session.");
    size_t message_length = strlen(buffer);
    // strcat(buffer, "\n Recipient: ");

    size_t bytes_sent;
    if ((bytes_sent = send(client_handler, buffer, BUFFER_SIZE, 0)) < message_length)
    {
        fprintf(stderr, "%ld / %ld bytes sent", bytes_sent, message_length);
        perror("Error sending the data");
    }

    read_client_responses(client_handler);
    // TODO implement the interface in this part, not in the client one

    // Free allocated memory
    free(client_handler_ptr);
}

void listen_incoming_connections(void *server_struct_ptr_arg)
{
    uniSocket *server_struct_ptr = (uniSocket *)server_struct_ptr_arg;

    while (true)
    {
        int client_handler = acceptConnection(server_struct_ptr->sock_fd,
                                              (struct sockaddr*)server_struct_ptr->address.addr_ipv4,
                                              &(server_struct_ptr->addrlen));

        // Allocate memory for the pointer
        int *cli_handler_ptr = malloc(sizeof(int));
        if (cli_handler_ptr == NULL)
        {
            perror("Error allocating memory for client handler pointer");
            exit(EXIT_FAILURE);
        }
        *cli_handler_ptr = client_handler;

        pthread_t handler_thread;
        pthread_create(&handler_thread, NULL, 
            handle_client_requests, (void *)cli_handler_ptr);
    }
}

void create_listening_thread(uniSocket *server_struct_ptr)
{
    // Creating the thread
    pthread_t listener_thread;
    int thread_create_status = pthread_create(&listener_thread, NULL, 
        listen_incoming_connections, (void *)server_struct_ptr);
    if (thread_create_status)
    {
        fprintf(stderr, "Value return from the thread creation is %d\n", thread_create_status);
        exit(EXIT_FAILURE);
    }

    // Allocating memory for the exit status
    void *status = malloc(sizeof(int));
    if (status == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for thread status\n");
        exit(EXIT_FAILURE);
    }
    //
    int join_status = pthread_join(listener_thread, &status);
    if (join_status != 0)
    {
        fprintf(stderr, "Error joining listener thread: %d\n", join_status);
        exit(EXIT_FAILURE);
    }

    // Checking the return status
    // TODO use this to store error values in the database
    int status_val = *((int *)status);
    if (status_val != 0)
    {
        fprintf(stderr, "Listener thread returned value %d\n", status_val);
    }
}

void start_server(int port)
{
    uniSocket *server_struct_ptr = create_socket(true, port, true);
    char buffer[BUFFER_SIZE];

    // Perpetuate thread to actively listen incoming connections
    create_listening_thread(server_struct_ptr);

    close_socket(server_struct_ptr);
    shutdown(server_struct_ptr->sock_fd, SHUT_RDWR); // TODO implement in the close server function
    puts(YELLOW "Closed" RESET);
}

int main(int argc, char *argv[])
{
    puts(YELLOW "Powering up the server!" RESET);
    start_server(PORT);

    return 0;
}
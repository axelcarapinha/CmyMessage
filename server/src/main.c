#include "net_utils_tcp.h"

#define PORT 8088
#define MAX_NUM_CLIENTS 2

void handle_client_requests(int client_handler)
{
    char buffer[BUFFER_SIZE];

    // TODO read contents from a file (interface part)


    send(client_handler, buffer, BUFFER_SIZE, 0);
}

void start_server()
{
    uniSocket *server_struct_ptr = create_socket(true, PORT, true);

    for (;;)
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
    puts("Closed");
}

void main()
{
    puts("Powering the server");
    start_server();

    //TODO use the shutdown RW instead of just closing the socket
    // close_server();

    return 0;
}
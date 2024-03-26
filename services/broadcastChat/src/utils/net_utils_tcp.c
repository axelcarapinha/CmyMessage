#include <asm-generic/socket.h>
#include "broadcast_chat_service.h"

/**
 * @brief
 *
 * @param a
 */
void close_server_socket(uniSocket *socket_struct_ptr)
{
    shutdown(socket_struct_ptr->sock_fd, SHUT_RDWR);
    close(socket_struct_ptr->sock_fd);
    free(socket_struct_ptr);
    puts(YELLOW "Server terminated." RESET);
}

/**
 * @brief
 *
 * @param a
 */
void setupServer(int opt, uniSocket *socket_struct_ptr)
{
    // Forcefully attaching socket to the port (part 1)
    if (setsockopt(socket_struct_ptr->sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Name the socket
    int bind_value;
    if (socket_struct_ptr->is_ipv4)
    {
        bind_value = bind(socket_struct_ptr->sock_fd, (struct sockaddr *)socket_struct_ptr->address.addr_ipv4, sizeof(*(socket_struct_ptr->address.addr_ipv4)));
    }
    else
    {
        bind_value = bind(socket_struct_ptr->sock_fd, (struct sockaddr *)socket_struct_ptr->address.addr_ipv6, sizeof(*(socket_struct_ptr->address.addr_ipv6)));
    }
    //
    if (bind_value < 0)
    {
        perror("bind failed");
        free(socket_struct_ptr);
        exit(EXIT_FAILURE);
    }
    else
    {
        puts(YELLOW "Binded." RESET);
    }

    // Listen to ports
    if (listen(socket_struct_ptr->sock_fd, DEFAULT_MAX_NUM_CLIENTS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    else
    {
        puts(YELLOW "Listening..." RESET);
    }

    puts(YELLOW "Server socket listening..." RESET);
}

/**
 * @brief
 *
 * @param a
 */
int create_descriptor(uniSocket *socket_struct_ptr)
{
    int file_descriptor;
    if (socket_struct_ptr->is_ipv4)
    {
        file_descriptor = socket(
            socket_struct_ptr->address.addr_ipv4->sin_family,
            SOCK_STREAM, 0);
    }
    else
    {
        file_descriptor = socket(
            socket_struct_ptr->address.addr_ipv6->sin6_family,
            SOCK_STREAM, 0);
    }

    // Verification
    if ((file_descriptor) < 0)
    {
        perror("Socket creation FAILED");
        exit(EXIT_FAILURE);
    }
    else
    {
        puts(YELLOW "Stream socket created SUCCESSFULY" RESET);
    }

    return file_descriptor;
}

/**
 * @brief
 *
 * @param a
 */
void initialize(uniSocket *socket_struct_ptr)
{
    if (socket_struct_ptr->is_ipv4)
    {
        socket_struct_ptr->address.addr_ipv4->sin_family = AF_INET;
        socket_struct_ptr->address.addr_ipv4->sin_addr.s_addr = INADDR_ANY;
        socket_struct_ptr->address.addr_ipv4->sin_port = htons(socket_struct_ptr->port);
    }
    else
    {
        socket_struct_ptr->address.addr_ipv6->sin6_family = AF_INET6;
        socket_struct_ptr->address.addr_ipv6->sin6_addr = in6addr_any;
        socket_struct_ptr->address.addr_ipv6->sin6_port = htons(socket_struct_ptr->port);
    }
}

/**
 * @brief
 *
 * @param
 */
uniSocket *create_socket(bool is_server_arg, int port, bool is_ipv4_arg)
{
    // UniSocket memory allocation
    // Using the pointer because the malloc function could
    // rearrange the memory positions,
    // and we would be accessing unintended data
    uniSocket *socket_struct_ptr = (uniSocket *)malloc(sizeof(uniSocket));
    if (socket_struct_ptr == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Address memory allocation
    socket_struct_ptr->is_ipv4 = is_ipv4_arg;
    if (is_ipv4_arg)
    {
        socket_struct_ptr->address.addr_ipv4 = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
        if (socket_struct_ptr->address.addr_ipv4 == NULL)
        {
            perror("Allocating memory for the ipv4 socket address");
            free(socket_struct_ptr);
            exit(EXIT_FAILURE);
        }
        //
        socket_struct_ptr->addrlen = sizeof(*(socket_struct_ptr->address.addr_ipv4));
    }
    else
    {
        socket_struct_ptr->address.addr_ipv6 = (struct sockaddr_in6 *)malloc(sizeof(struct sockaddr_in6));
        if (socket_struct_ptr->address.addr_ipv6 == NULL)
        {
            perror("Allocating memory for the ipv6 socket address");
            free(socket_struct_ptr);
            exit(EXIT_FAILURE);
        }
        //
        socket_struct_ptr->addrlen = sizeof(*(socket_struct_ptr->address.addr_ipv6));
    }

    // Other general settings
    socket_struct_ptr->port = port;
    socket_struct_ptr->is_server = is_server_arg;
    initialize(socket_struct_ptr);
    socket_struct_ptr->sock_fd = create_descriptor(socket_struct_ptr);

    // SERVER specific settings
    if (socket_struct_ptr->is_server)
    {
        int opt = 1;
        setupServer(opt, socket_struct_ptr);
    }

    return socket_struct_ptr;
}

/**
 * @brief
 *
 * @param a
 */
//TODO change the name of this function to handle the memoru management
ClientInfo *acceptConnection(int echo_server, struct sockaddr *cli_address, socklen_t *cli_addrlen)
{
    // Allocate memory for the client information
    ClientInfo *client_struct_ptr = (ClientInfo *)malloc(sizeof(ClientInfo));
    if (client_struct_ptr == NULL)
    {
        printf("Problem allocating memory for the client struct\n");
        return NULL;
    }
    //
    client_struct_ptr->client_addr_ptr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    if (client_struct_ptr->client_addr_ptr == NULL) {
        printf("Problem allocating memory for the client's address struct\n");
        free(client_struct_ptr);
        return NULL;
    }
    //
    client_struct_ptr->client_addr_len_ptr = (socklen_t *)malloc(sizeof(socklen_t));
    if (client_struct_ptr->client_addr_len_ptr == NULL)
    {
        printf("Problem allocating memory for the client's address length\n");
        free(client_struct_ptr);
        return NULL;
    }

    // Accept the connection
    int client_handler_socket = -1;

    if ((client_handler_socket = accept(echo_server, (struct sockaddr *)cli_address, cli_addrlen)) < 0)
    {
        perror("Problem accepting client's connection");
        exit(EXIT_FAILURE);
    }
    //
    client_struct_ptr->client_handler_FD = client_handler_socket;

    return client_struct_ptr;
}

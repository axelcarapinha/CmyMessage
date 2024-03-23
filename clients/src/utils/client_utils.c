#include "client_utils.h"
#include <asm-generic/socket.h>

/**
 * @brief
 *
 * @param a
 */
void close_socket(uniSocket *socket_struct_ptr)
{
    close(socket_struct_ptr->sock_fd);
}

/**
 * @brief
 *
 * @param 
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
 * @param a
 */
uniSocket *create_socket(bool is_server_arg, int port, bool is_ipv4_arg)
{
    // UniSocket memory allocation
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

    return socket_struct_ptr;
}

/**
 * @brief
 *
 * @param a
 */
int acceptConnection(int echo_server, struct sockaddr *address, socklen_t *addrlen)
{
    int client_socket = -1;

    if ((client_socket = accept(echo_server, (struct sockaddr *)address, addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    return client_socket;
}

/**
 * @brief
 *
 * @param a
 */
void connect_cli(uniSocket *cli_struct_ptr, int port)
{
    // Address settings
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    //
    if (inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) <= 0)
    {
        perror("Unsupported address.");
        exit(EXIT_FAILURE);
    }

    //TODO consider this piece of code
    // bcopy(
    //     (char *)server->h_addr,
    //     (char *)&servaddr.sin_addr.s_addr,
    //     server->h_length
    // );

    // Connect to the server
    int cli_socketFD;
    if ((cli_socketFD = connect(cli_struct_ptr->sock_fd,
                                (struct sockaddr *)cli_struct_ptr->address.addr_ipv4, cli_struct_ptr->addrlen)) >= 0)
    {
        cli_struct_ptr->sock_fd = cli_socketFD;
    }
    else
    {
        perror("Connection to the server failed");
        exit(EXIT_FAILURE);
    }
}

#include <asm-generic/socket.h>
#include "broadcast_chat_service.h"

/**
 * @brief Gracefully close the service socket
 *
 * @param p_socket_t
 */
void close_service_socket(UniSocket_t *p_socket_t)
{
    shutdown(p_socket_t->sock_fd, SHUT_RDWR);
    close(p_socket_t->sock_fd);
    free(p_socket_t);
    puts(YELLOW "Server terminated." RESET);
}



/**
 * @brief Gracefully close the service socket
 *
 * @param p_socket_t
 */
void free_unisocket_struct_mem(UniSocket_t *p_socket_t)
{
    if (p_socket_t == NULL)
        return;

    // Address
    if (p_socket_t->is_ipv4)
    {
        if (p_socket_t->address.p_addr_ipv4 != NULL)
        {
            free(p_socket_t->address.p_addr_ipv4);
        }
    }
    else
    {
        if (p_socket_t->address.p_addr_ipv6 != NULL)
        {
            free(p_socket_t->address.p_addr_ipv6);
        }
    }

    // Thread related
    if (p_socket_t->p_thread_pool != NULL)
    {
        free(p_socket_t->p_thread_pool);
    }
    if (p_socket_t->p_mutex_queue == NULL)
    {
        free(p_socket_t->p_mutex_queue);
    }
    if (p_socket_t->p_condition_var == NULL)
    {
        free(p_socket_t->p_condition_var);
    }

    // Struct pointer itself
    free(p_socket_t);
}

/**
 * @brief
 *
 * @param a
 */
ClientInfo *acceptConnection(int service_FD)
{
    ClientInfo *p_client_t = allocate_client_info_struct();
    struct sockaddr *address = p_client_t->p_ipv4_addr;
    socklen_t addr_len = *(p_client_t->p_addr_len);

    int client_handler_FD;
    if ((client_handler_FD = accept(service_FD, address, addr_len)) < 0)
    {
        free_client_mem(p_client_t);
        perror("Problem accepting client's connection");
        int error_val = client_handler_FD;
        return error_val;
    }
    //
    p_client_t->client_handler_FD = client_handler_socket;

    return p_client_t;
}

/**
 * @brief
 *
 * @param
 */
int setup_service_socket_t(int opt, UniSocket_t *p_socket_t)
{
    // Forcefully attaching socket to the port
    if (setsockopt(p_socket_t->sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        close_service_socket(p_socket_t);
        perror("Setting up the service options");
        return -1;
    }

    // Name the socket
    int bind_status;
    //
    struct sockaddr *address;
    socklen_t addr_len;
    if (p_socket_t->is_ipv4) {
        address = (struct sockaddr *)p_socket_t->addr_u.p_ipv4;
        addr_len = sizeof(*p_socket_t->addr_u.p_ipv4);
    }
    else {
        address = (struct sockaddr *)p_socket_t->addr_u.p_ipv6;
        addr_len = sizeof(*p_socket_t->addr_u.p_ipv6);
    }
    //
    if (bind_status = bind(p_socket_t->sock_fd, address, addr_len) < 0) {
        free_unisocket_mem(p_socket_t);
        perror("Service socket binding");
        return bind_status;
    }

    // Listen to ports
    int listen_status;
    if (listen_status = listen(p_socket_t->sock_fd, DEFAULT_MAX_NUM_CLIENTS) < 0)
    {
        free_unisocket_mem(p_socket_t);
        perror("Service socket, problem on listening");
        return listen_status;
    }

    printf(YELLOW "Service socket listening...\n" RESET);
}

/**
 * @brief
 *
 * @param a
 */
int assign_descriptor_to_stream_socket_t(UniSocket_t *p_socket_t)
{
    const int ADDR_FAMILY = (p_socket_t->is_ipv4) ? AF_INET : AF_INET6;

    int socket_descriptor;
    if ((socket_descriptor = socket(ADDR_FAMILY, SOCK_STREAM, 0)) < 0)
    {
        perror("invalid socket_descriptor");
        int error_value = socket_descriptor;
        return error_value;
    }

    // Assign the file descriptor
    p_socket_t->sock_fd = socket_descriptor;
    printf(YELLOW "Stream socket created SUCCESSFULY\n" RESET );
}

/**
 * @brief
 *
 * @param a
 */
int initialize_socket(UniSocket_t *p_socket_t)
{
    if (p_socket_t->is_ipv4)
    {
        // If the system or network configs does NOT support dual-stack sockets,
        // this approach, while NOT recommended, allows the server to accept only IPv4 connections.
        // Noteworthy, if the server has IPv4 and IPv6 sockets,
        // please, consider changing opt to 1, for a more controlled flow of the connections
        p_socket_t->addr_u.p_ipv4->sin_family = AF_INET;
        p_socket_t->addr_u.p_ipv4->sin_addr.s_addr = INADDR_ANY;
        p_socket_t->addr_u.p_ipv4->sin_port = htons(p_socket_t->port);
    }
    else
    {
        // Dual-stack socket
        int opt = 0; // Option value to turn off IPV6_V6ONLY
        if (setsockopt(p_socket_t->sock_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) < 0)
        {
            perror("setsockopt IPV6_V6ONLY");
            return -1;
        }
        p_socket_t->addr_u.p_ipv6->sin6_family = AF_INET6;
        p_socket_t->addr_u.p_ipv6->sin6_addr = in6addr_any;
        p_socket_t->addr_u.p_ipv6->sin6_port = htons(p_socket_t->port);
    }
}

/**
 * @brief
 *
 * @param a
 */
ClientInfo *allocate_client_info_struct()
{
    ClientInfo *p_client_t = (ClientInfo *)malloc(sizeof(ClientInfo));
    if (p_client_t == NULL)
    {
        perror("Allocating memory for the client struct");
        return NULL;
    }
    //
    p_client_t->client_addr_ptr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    if (p_client_t->client_addr_ptr == NULL)
    {
        free(p_client_t);
        perror("Allocating memory for the client address on the struct");
        return NULL;
    }
    //
    p_client_t->client_addr_len_ptr = (socklen_t *)malloc(sizeof(socklen_t));
    if (p_client_t->client_addr_len_ptr == NULL)
    {
        free(p_client_t->client_addr_ptr);
        free(p_client_t);
        perror("Allocating memory for the client's address length");
        return NULL;
    }

    return p_client_t;
}

UniSocket_t *allocate_socket_struct()
{
    UniSocket_t *p_socket_t = (UniSocket_t *)malloc(sizeof(UniSocket_t));

    if (p_socket_t == NULL)
    {
        perror("Socket memory allocation failed");
        return NULL;
    }

    // Address
    p_socket_t->addr_u.p_ipv6 = (struct sockaddr_in6 *)malloc(sizeof(struct sockaddr_in6));
    if (p_socket_t->addr_u.p_ipv6 == NULL)
    {
        free_unisocket_mem(p_socket_t);
        perror("Socket address creation failed");
        return NULL;
    }
    //
    p_socket_t->p_addr_len = (struct socklen_t *)malloc(sizeof(socklen_t));
    if (p_socket_t->p_addr_len == NULL)
    {
        free_unisocket_mem(p_socket_t);
        perror("Socket address creation failed");
        return NULL;
    }

    // Service pointer
    p_socket_t->p_service_func = (ServiceFunctionPtr *)malloc(sizeof(ServiceFunctionPtr));
    if (p_socket_t->p_service_func == NULL)
    {
        free_unisocket_mem(p_socket_t);
        perror("Socket service function allocation failed");
        return NULL;
    }

    return p_socket_t;
}

/**
 * @brief
 *
 * @param
 */
UniSocket_t *create_socket_struct(bool is_server_arg, int port, bool is_ipv4_arg)
{
    UniSocket_t *p_socket_t;
    if (p_socket_t = allocate_socket_struct() == NULL) {
        perror("Socket struct memory allocation");
        return NULL;
    }

    // GENERAL settings
    p_socket_t->is_server = is_server_arg;
    p_socket_t->port = port;
    p_socket_t->is_ipv4 = is_ipv4_arg;
    //
    int init_status;
    if (init_status = initialize_socket(p_socket_t) < 0) {
        return init_status;
    }
    //
    int assign_status;
    if (assign_status = assign_descriptor_to_stream_socket_t(p_socket_t) < 0) {
        perror("Assigning descriptor to the socket");
        return assign_status;
    }

    // SERVER specific settings
    if (p_socket_t->is_server)
    {
        int option = 1;
        int setup_status;
        if (setup_status = setup_service_socket_t(option, p_socket_t) < 0) {
            perror("Failed to set up the server socket");
            return setup_status;
        }
    }

    return p_socket_t;
}
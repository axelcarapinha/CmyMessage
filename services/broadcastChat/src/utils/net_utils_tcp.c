#include <asm-generic/socket.h>
#include "broadcast_chat_service.h"

/**
 * @brief Gracefully closes the service socket
 *
 * This function shuts down and closes the socket file descriptor, deallocates memory associated with the socket structure,
 * and prints a message indicating that the server has been closed.
 *
 * @param p_socket_t Pointer to the UniSocket_t struct representing the socket (client or service)
 */
void close_server_socket(UniSocket_t *p_socket_t)
{
    shutdown(p_socket_t->sock_FD, SHUT_RDWR);
    close(p_socket_t->sock_FD);
    free_unisocket_memory(p_socket_t);
    puts(YELLOW "Server closed." RESET);
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Deallocate all memory related to a SOCKET structure
 *
 * This function deallocates all memory associated with a socket structure, including address-related memory
 * and the structure itself. It checks for NULL pointers to avoid dereferencing invalid memory.
 *
 * @param p_socket_t Pointer to the UniSocket_t struct representing the socket (client or service)
 */
void free_unisocket_memory(UniSocket_t *p_socket_t)
{
    // No memory deallocation is needed
    if (p_socket_t == NULL)
        return;

    // Address related memory
    if (p_socket_t->is_ipv4)
    {
        if (p_socket_t->addr_u.p_ipv4 != NULL)
            free(p_socket_t->addr_u.p_ipv4);
    }
    else if (p_socket_t->addr_u.p_ipv6 != NULL)
    {
        free(p_socket_t->addr_u.p_ipv6);
    }

    // Struct pointer itself
    free(p_socket_t);
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Deallocate all memory related to a CLIENT structure
 *
 * This function deallocates all memory associated with a socket structure, including address-related memory
 * and the structure itself. It checks for NULL pointers to avoid dereferencing invalid memory.
 *
 * @param p_client_t Pointer to the ClientInfo_t struct
 */
void free_client_memory(ClientInfo_t *p_client_t)
{
    ensure_client_disconnection_and_remove_data(p_client_t);

    // No memory deallocation is needed
    if (p_client_t == NULL)
        return;

    // Address related memory
    if (p_client_t->p_addr != NULL)
        free(p_client_t->p_addr);
    if (p_client_t->p_addr_len != NULL)
        free(p_client_t->p_addr_len);

    // Other memoy deallocations
    if (p_client_t->buffer != NULL)
        free(p_client_t->buffer);

    // Struct pointer itself
    free(p_client_t);
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Ensures client disconnection and removes sensitive data.
 *
 * This function closes the socket associated with the client and clears sensitive
 * information stored in the ClientInfo_t structure to maintain privacy and security.
 *
 * @param p_client_t Pointer to the ClientInfo_t structure representing the client.
 */
void ensure_client_disconnection_and_remove_data(ClientInfo_t *p_client_t) // In case the service did NOT
{
    printf("Cleaning ALL the of %s's connection data.\n\n", p_client_t->name);

    close(p_client_t->sock_FD);
    //
    if (p_client_t->buffer != NULL) 
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
    if (p_client_t->name != NULL)
        memset(p_client_t->name, 0, strlen(p_client_t->name));
    if (p_client_t->addr_info != NULL)
        memset(p_client_t->addr_info, 0, strlen(p_client_t->addr_info));
    if (p_client_t->recipient != NULL)
        memset(p_client_t->recipient, 0, strlen(p_client_t->recipient));
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Allocate CLIENT information structure
 *
 * This function dynamically allocates memory for a ClientInfo_t structure,
 * which contains information related to a client.
 * It first allocates memory for the main structure itself,
 * then allocates memoy for the client's address (p_addr)
 * and the length of the address (p_addr_len).
 * If any memory allocation fails, it prints an error message using perror
 * and frees any previously allocated memory before returning NULL.
 * Finally, it returns a pointer to the allocated ClientInfo_t structure
 * if all allocations are successful.
 *
 * @return Pointer to the allocated ClientInfo_t structure if successful, otherwise NULL
 */
ClientInfo_t *allocate_client_info_struct()
{
    ClientInfo_t *p_client_t = (ClientInfo_t *)malloc(sizeof(ClientInfo_t));
    if (p_client_t == NULL)
    {
        perror("Error allocating memory for the client struct");
        return NULL;
    }
    //
    p_client_t->p_addr = (struct sockaddr *)malloc(sizeof(struct sockaddr));
    if (p_client_t->p_addr == NULL)
    {
        free_client_memory(p_client_t);
        perror("Error allocating memory for the client address on the struct");
        return NULL;
    }
    //
    p_client_t->p_addr_len = (socklen_t *)malloc(sizeof(socklen_t));
    if (p_client_t->p_addr_len == NULL)
    {
        free_client_memory(p_client_t);
        perror("Error allocating memory for the client's address length");
        return NULL;
    }

    return p_client_t;
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Accept client CONNECTION
 *
 * This function accepts an incoming connection on a given socket file descriptor (service_FD).
 * It allocates memory for a ClientInfo_t structure to hold information
 * about the client.
 * Also, it attempts to accept the connection using the accept() function,
 * storing the client's address information in the allocated structure.
 * If the connection acceptance fails, it frees the allocated memory and prints an
 * error message using perror. Finally, it returns a pointer to the allocated
 * ClientInfo_t structure if the connection is successfully accepted, otherwise NULL.
 *
 * @param service_FD The file descriptor of the listening socket
 * @return Pointer to the allocated ClientInfo_t structure if connection is accepted successfully, otherwise NULL
 */
ClientInfo_t *accept_connection(int service_FD)
{
    ClientInfo_t *p_client_t = allocate_client_info_struct();
    struct sockaddr *p_cli_addr = p_client_t->p_addr;
    socklen_t addr_len = sizeof(struct sockaddr);

    // Accept incoming connection
    int client_FD;
    if ((client_FD = accept(service_FD, p_cli_addr, &addr_len)) < 0)
    {
        free_client_memory(p_client_t);
        perror("Error accepting client's connection");
        return NULL;
    }

    // Assign the file descriptor to the client
    p_client_t->sock_FD = client_FD;

    return p_client_t;
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Setup SERVICE socket
 *
 * This function configures the service socket for listening on a given port
 * with certain options.
 * It then binds the socket to the address specified in the UniSocket_t structure
 * and starts listening for incoming connections.
 * If any of these steps fail, it frees the allocated memory and prints an error message
 * using perror. It returns 0 on success and the appropriate error code otherwise.
 *
 * @param opt The option value for setting socket options
 * @param p_socket_t Pointer to the UniSocket_t structure representing the socket
 * @return 0 on success, or -1 if any error occurs during setup
 */
int setup_service_socket_t(int opt, UniSocket_t *p_socket_t)
{
    // Forcefully attaching socket to the port
    if (setsockopt(p_socket_t->sock_FD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        close_server_socket(p_socket_t);
        perror("Error setting up the service options");
        return -1;
    }

    // Name the socket
    int bind_status;
    //
    struct sockaddr *address;
    socklen_t addr_len;
    if (p_socket_t->is_ipv4)
    {
        address = (struct sockaddr *)p_socket_t->addr_u.p_ipv4;
        addr_len = sizeof(*p_socket_t->addr_u.p_ipv4);
    }
    else
    {
        address = (struct sockaddr *)p_socket_t->addr_u.p_ipv6;
        addr_len = sizeof(*p_socket_t->addr_u.p_ipv6);
    }
    //
    if ((bind_status = bind(p_socket_t->sock_FD, address, addr_len)) < 0)
    {
        free_unisocket_memory(p_socket_t);
        perror("Error binding the socket");
        return bind_status;
    }

    // Listen to ports
    int listen_status;
    if ((listen_status = listen(p_socket_t->sock_FD, DEFAULT_MAX_NUM_CLIENTS)) < 0)
    {
        free_unisocket_memory(p_socket_t);
        perror("Error putting socket on listening state");
        return listen_status;
    }

    printf(YELLOW "Service socket listening...\n" RESET);
    return 0;
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Assign DESCRIPTOR to a stream socket
 *
 * This function creates a stream socket descriptor and assigns it to the UniSocket_t structure.
 * It determines the address family (IPv4 or IPv6) based on the is_ipv4 flag in the UniSocket_t structure.
 * If the socket creation fails, it prints an error message using perror and returns the error code.
 * Otherwise, it assigns the created socket descriptor to the sock_FD field of the UniSocket_t structure
 * and prints a success message. It returns 0 on success.
 *
 * @param p_socket_t Pointer to the UniSocket_t structure representing the socket
 * @return 0 on success, or the error code if socket creation fails
 */
int assign_descriptor_to_stream_socket_t(UniSocket_t *p_socket_t)
{
    const int ADDR_FAMILY = (p_socket_t->is_ipv4) ? AF_INET : AF_INET6;

    int socket_descriptor;
    if ((socket_descriptor = socket(ADDR_FAMILY, SOCK_STREAM, 0)) < 0)
    {
        perror("Error creating socket descriptor");
        int error_value = socket_descriptor;
        return error_value;
    }

    // Assign the file descriptor
    p_socket_t->sock_FD = socket_descriptor;
    printf(YELLOW "Stream socket created SUCCESSFULY\n" RESET);

    return 0;
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Initialize socket
 *
 * This function initializes the socket address structure based on the UniSocket_t structure.
 * If the UniSocket_t structure represents an IPv4 socket, it initializes the address structure
 * for IPv4. Otherwise, it initializes it for a dual-stack IPv6 socket (recommended option).
 *
 * For IPv4 sockets, it sets the address family, IP address, and port based on the UniSocket_t
 * structure's values. For IPv6 sockets, it sets the IPV6_V6ONLY option to allow both IPv4 and IPv6
 * connections on the same socket (if supported by the system), and then initializes the address
 * structure with the in6addr_any and the specified port.
 *
 * @param p_socket_t Pointer to the UniSocket_t structure representing the socket
 * @return 0 on success, or -1 if setting socket options fails for IPv6 sockets
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
        *(p_socket_t->p_addr_len) = sizeof(p_socket_t->addr_u.p_ipv4);
    }
    else
    {
        // Dual-stack socket
        int opt = 0; // Option value to turn off IPV6_V6ONLY
        if (setsockopt(p_socket_t->sock_FD, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) < 0)
        {
            perror("Error setting socket options");
            return -1;
        }
        p_socket_t->addr_u.p_ipv6->sin6_family = AF_INET6;
        p_socket_t->addr_u.p_ipv6->sin6_addr = in6addr_any;
        p_socket_t->addr_u.p_ipv6->sin6_port = htons(p_socket_t->port);
        *(p_socket_t->p_addr_len) = sizeof(p_socket_t->addr_u.p_ipv6);
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Allocate SOCKET structure
 *
 * This function allocates memory for a UniSocket_t structure along with its associated address
 * structure and length variable. Additionally, it allocates memory for a service function pointer.
 *
 * If memory allocation fails at any point during the process, it frees the previously allocated
 * memory and prints an error message using perror(), indicating the specific allocation failure.
 *
 * @return A pointer to the allocated UniSocket_t structure on success with allocation, otherwise NULL
 */
UniSocket_t *allocate_socket_struct()
{
    UniSocket_t *p_socket_t = (UniSocket_t *)malloc(sizeof(UniSocket_t));

    if (p_socket_t == NULL)
    {
        perror("Error allocating memory for the struct");
        return NULL;
    }

    // Address
    p_socket_t->addr_u.p_ipv6 = (struct sockaddr_in6 *)malloc(sizeof(struct sockaddr_in6));
    if (p_socket_t->addr_u.p_ipv6 == NULL)
    {
        free_unisocket_memory(p_socket_t);
        perror("Error allocating memory for the address variable");
        return NULL;
    }
    //
    p_socket_t->p_addr_len = (socklen_t *)malloc(sizeof(socklen_t));
    if (p_socket_t->p_addr_len == NULL)
    {
        free_unisocket_memory(p_socket_t);
        perror("Error allocating memory for the address lenght variable");
        return NULL;
    }

    // Service pointer
    p_socket_t->p_service_func = (ServiceFunctionPtr)malloc(sizeof(ServiceFunctionPtr));
    if (p_socket_t->p_service_func == NULL)
    {
        free_unisocket_memory(p_socket_t);
        perror("Error allocating memory for the service function");
        return NULL;
    }

    return p_socket_t;
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief Create socket structure
 *
 * This function creates and initializes a UniSocket_t structure based on the provided parameters.
 * It allocates memory for the structure, initializes its general settings, assigns a socket descriptor,
 * and performs server-specific settings if the socket is intended to be a server.
 *
 * @param is_server_arg Flag indicating whether the socket is for a server (true) or a client (false)
 * @param port The port number for the socket
 * @param is_ipv4_arg Flag indicating whether the socket should use IPv4 (true) or IPv6 (false),
 * being the last option the recommended for the server side
 *
 * @return A pointer to the created and initialized UniSocket_t structure on success, or NULL if any error occurs
 */
UniSocket_t *create_socket_struct(bool is_server_arg, int port, bool is_ipv4_arg)
{
    UniSocket_t *p_socket_t;
    if ((p_socket_t = allocate_socket_struct()) == NULL)
    {
        perror("Error allocating memory for the socket struct");
        return NULL;
    }

    // GENERAL settings
    p_socket_t->is_server = is_server_arg;
    p_socket_t->port = port;
    p_socket_t->is_ipv4 = is_ipv4_arg;
    //
    if (initialize_socket(p_socket_t) < 0)
    {
        perror("Error initializing the socket");
        free_unisocket_memory(p_socket_t);
        return NULL;
    }
    //
    if (assign_descriptor_to_stream_socket_t(p_socket_t) < 0)
    {
        perror("Error assigning descriptor to the socket");
        free_unisocket_memory(p_socket_t);
        return NULL;
    }

    // SERVER specific settings
    if (p_socket_t->is_server)
    {
        int option = 1;
        if (setup_service_socket_t(option, p_socket_t) < 0)
        {
            perror("Error setting up the server socket");
            free_unisocket_memory(p_socket_t);
            return NULL;
        }
    }

    return p_socket_t;
}
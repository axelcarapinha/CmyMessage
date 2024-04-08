#include "ftp_service.h"

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 *
 * @param
 */
void close_socket_with_ptr_if_open(int *p_socket_descriptor)
{
    if (*p_socket_descriptor != -1)
    {
        int closing_status;
        if ((closing_status = close(*p_socket_descriptor)) < 0)
        {
            ERROR_VERBOSE_LOG("Error closing socket");
        }
        //
        *p_socket_descriptor = -1; // Ensure that the socket is closed only once
    }
}

//----------------------------------------------------------------------------------------------------------

/**
 * @brief
 *

 *
 * @param
 */
void close_server_socket(UniSocket_t *p_socket_t)
{
    shutdown(p_socket_t->sock_FD, SHUT_RDWR);
    close_socket_with_ptr_if_open(&p_socket_t->sock_FD);
    free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);

    INFO_VERBOSE_LOG(YELLOW "Server closed." RESET);    
}


//----------------------------------------------------------------------------------------------------------
/**
* @brief
*
*
* @param
*/
void sigsegv_handler(int signum) {
    ERROR_VERBOSE_LOG(RED "Ignored SEGFAULT possibly originated by freeing memory more than once. Continuing the execution..." RESET);
    return;
}

//----------------------------------------------------------------------------------------------------------
/**
* @brief
*
*
* @param
*/

void free_server_socket_memory_with_ptr_to_ptr(void **p_p_socket_t_arg)
{
    // Avoid shutdown of the server because of trying to deallocated memory repeatedly
    // even though there are measures to avoid that in this function
    signal(SIGSEGV, sigsegv_handler);

    // Allow compatibility with hash generalization
    UniSocket_t **p_p_socket_t = (UniSocket_t **)p_p_socket_t_arg;

    // No memory deallocation is needed
    if (*p_p_socket_t == NULL)
        return;

    // Address related memory
    if ((*p_p_socket_t)->is_ipv4)
    {
        if ((*p_p_socket_t)->addr_u.p_ipv4 != NULL)
        {
            free((*p_p_socket_t)->addr_u.p_ipv4);
            (*p_p_socket_t)->addr_u.p_ipv4 = NULL;
        }
    }
    else if ((*p_p_socket_t)->addr_u.p_ipv6 != NULL)
    {
        free((*p_p_socket_t)->addr_u.p_ipv6);
        (*p_p_socket_t)->addr_u.p_ipv6 = NULL;
    }

    // TODO UNDERSTAND why it works with this order (troquei again)
    if (*p_p_socket_t != NULL)
    {
        *p_p_socket_t = NULL;
        free(*p_p_socket_t);
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Free memory associated with a ClientInfo_t structure.
 *
 * This function frees memory associated with a ClientInfo_t structure and its members.
 * It sets up a signal handler to avoid server shutdown due to repeated deallocation attempts.
 * It checks if the pointer to the pointer to the client structure is NULL or if the client
 * structure itself is NULL to prevent dereferencing NULL pointers. Then, it frees memory
 * associated with the client's address (p_addr), address length (p_addr_len), and socket buffer.
 * Finally, it sets the pointer to the client structure to NULL and frees the (allocated) client structure itself.
 *
 * @param p_p_client_t_arg Pointer to a pointer to the ClientInfo_t structure.
 */


void free_client_memory_with_ptr_to_ptr(void **p_p_client_t_arg)
{
    // Avoid shutdown of the server because of trying to deallocated memory repeatedly
    // even though there are measures to avoid that in this function
    signal(SIGSEGV, sigsegv_handler);

    // Allow compatibility with hash generalization
    ClientInfo_t **p_p_client_t = (ClientInfo_t **)p_p_client_t_arg;

    if (p_p_client_t == NULL)
    {
        return;
    }

    // Check if the client struct was already deallocated from the heap
    if (p_p_client_t == NULL || *p_p_client_t == NULL)
    {
        return;
    }

    // Address related memory
    if ((*p_p_client_t)->p_addr != NULL)
    {
        free((*p_p_client_t)->p_addr);
        (*p_p_client_t)->p_addr = NULL;
    }
    if ((*p_p_client_t)->p_addr_len != NULL)
    {
        free((*p_p_client_t)->p_addr_len);
        (*p_p_client_t)->p_addr_len = NULL;
    }

    // Socket buffer deallocation
    if ((*p_p_client_t)->buffer != NULL)
    {
        free((*p_p_client_t)->buffer);
        (*p_p_client_t)->buffer = NULL;
    }

    // TODO UNDERSTAND why it works with this order
    if (*p_p_client_t != NULL)
    {
        *p_p_client_t = NULL;
        free(*p_p_client_t);
    }
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
        ERROR_VERBOSE_LOG("Error allocating memory for the client struct");
        return NULL;
    }
    //
    p_client_t->p_addr = (struct sockaddr *)malloc(sizeof(struct sockaddr));
    if (p_client_t->p_addr == NULL)
    {
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        ERROR_VERBOSE_LOG("Error allocating memory for the client address on the struct");
        return NULL;
    }
    //
    p_client_t->p_addr_len = (socklen_t *)malloc(sizeof(socklen_t));
    if (p_client_t->p_addr_len == NULL)
    {
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        ERROR_VERBOSE_LOG("Error allocating memory for the client's address length");
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
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        ERROR_VERBOSE_LOG("Error accepting client's connection");
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
        ERROR_VERBOSE_LOG("Error setting up the service options");
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

        free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
        ERROR_VERBOSE_LOG("Error binding the socket");
        return bind_status;
    }

    // Listen to ports
    int listen_status;
    if ((listen_status = listen(p_socket_t->sock_FD, MAX_NUM_CLIENTS)) < 0)
    {
        free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
        ERROR_VERBOSE_LOG("Error putting socket on listening state");
        return listen_status;
    }
    
    INFO_VERBOSE_LOG(YELLOW "Service socket listening...\n" RESET);
    
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
        ERROR_VERBOSE_LOG("Error creating socket descriptor");   
        int error_value = socket_descriptor;
        return error_value;
    }

    // Assign the file descriptor
    p_socket_t->sock_FD = socket_descriptor;
    INFO_VERBOSE_LOG(YELLOW "Stream socket created SUCCESSFULY\n" RESET);

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
            ERROR_VERBOSE_LOG("Error setting socket options");   
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
        ERROR_VERBOSE_LOG("Error allocating memory for the struct");
        return NULL;
    }

    // Address
    p_socket_t->addr_u.p_ipv6 = (struct sockaddr_in6 *)malloc(sizeof(struct sockaddr_in6));
    if (p_socket_t->addr_u.p_ipv6 == NULL)
    {
        free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
        ERROR_VERBOSE_LOG("Error allocating memory for the address variable");
        return NULL;
    }
    //
    p_socket_t->p_addr_len = (socklen_t *)malloc(sizeof(socklen_t));
    if (p_socket_t->p_addr_len == NULL)
    {
        free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
        ERROR_VERBOSE_LOG("Error allocating memory for the address lenght variable");
        return NULL;
    }

    // Service pointer
    p_socket_t->p_service_func = (ServiceFunctionPtr)malloc(sizeof(ServiceFunctionPtr));
    if (p_socket_t->p_service_func == NULL)
    {
        free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
        ERROR_VERBOSE_LOG("Error allocating memory for the service function");
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
        ERROR_VERBOSE_LOG("Error allocating memory for the socket struct");
        return NULL;
    }

    // GENERAL settings
    p_socket_t->is_server = is_server_arg;
    p_socket_t->port = port;
    p_socket_t->is_ipv4 = is_ipv4_arg;
    //
    if (initialize_socket(p_socket_t) < 0)
    {
        ERROR_VERBOSE_LOG("Error initializing the socket");
        free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
        return NULL;
    }
    //
    if (assign_descriptor_to_stream_socket_t(p_socket_t) < 0)
    {
        ERROR_VERBOSE_LOG("Error assigning descriptor to the socket");
        free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
        return NULL;
    }

    // SERVER specific settings
    if (p_socket_t->is_server)
    {
        int option = 1;
        if (setup_service_socket_t(option, p_socket_t) < 0)
        {
            ERROR_VERBOSE_LOG("Error setting up the server socket");
            free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
            return NULL;
        }
    }

    return p_socket_t;
}
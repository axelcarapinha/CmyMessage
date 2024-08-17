#include "ftp_service.h"

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

void close_server_socket(UniSocket_t *p_socket_t)
{
    shutdown(p_socket_t->sock_FD, SHUT_RDWR);
    close_socket_with_ptr_if_open(&p_socket_t->sock_FD);
    free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);

    INFO_VERBOSE_LOG(YELLOW "Server closed." RESET);    
}

void sigsegv_handler(int signum) {
    ERROR_VERBOSE_LOG(RED "Ignored SEGFAULT possibly originated by freeing memory more than once. Continuing the execution..." RESET);
    return;
}

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

    // TODO UNDERSTAND why it works with this order (changed again)
    if (*p_p_socket_t != NULL)
    {
        *p_p_socket_t = NULL;
        free(*p_p_socket_t);
    }
}

void free_client_memory_with_ptr_to_ptr(void **p_p_client_t_arg)
{
    // This detection of the SIGSEV avoids the shutdown of the server 
    // because of it trying to deallocate memory repeatedly
    // (even though there are measures to avoid that in this function)
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

//TODO use in the code for the server (allowing file transfering between servers, ...)
int connect_to_server(UniSocket_t *p_socket_t) {

    // Adapt to the desired server struct
    struct sockaddr *p_desired_address_t;
    socklen_t addr_len;
    if (p_socket_t->is_ipv4) {
        p_desired_address_t = (struct sockaddr *)p_socket_t->addr_u.p_ipv4;
        addr_len = sizeof(struct sockaddr_in);
    }
    else {
        p_desired_address_t = (struct sockaddr *)p_socket_t->addr_u.p_ipv6;
        addr_len = sizeof(struct sockaddr_in6);
    }

    // Connect
    int connection_status;
    if ((connection_status = connect(p_socket_t->sock_FD, p_desired_address_t, addr_len)) < 0) {
        perror("Error connecting to the desired server");
        return connection_status;
    }

    return 0;
}

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

int initialize_socket(UniSocket_t *p_socket_t)
{
    //! Consider reading the description in the header file,
    //! for the recommended choice of protocol when creating the socket
    if (p_socket_t->is_ipv4)
    {
        memset(p_socket_t->addr_u.p_ipv4, 0, sizeof(struct sockaddr_in)); // Clean the allocated memory for the structure
        p_socket_t->addr_u.p_ipv4->sin_family = AF_INET;
        *(p_socket_t->p_addr_len) = sizeof(*(p_socket_t->addr_u.p_ipv4));
        p_socket_t->addr_u.p_ipv4->sin_port = htons(p_socket_t->port);

        // Specify the pretended address
        if (p_socket_t->ip_address != NULL) {
            if (inet_pton(AF_INET, p_socket_t->ip_address, &(p_socket_t->addr_u.p_ipv4->sin_addr)) <= 0) {
                
                ERROR_VERBOSE_LOG("Error, invalid address for the client to connet with IPv4");
                return -1;
            }
        }
        else if (p_socket_t->is_server) {
            p_socket_t->addr_u.p_ipv4->sin_addr.s_addr = INADDR_ANY;
            //TODO consider specifying socket options
        }
        else {
            ERROR_VERBOSE_LOG("Error, invalid address for the client to connect with IPv6");
            return -2;
        }
    }
    else // Dual-stack socket
    {
        p_socket_t->addr_u.p_ipv6->sin6_family = AF_INET6;
        *(p_socket_t->p_addr_len) = sizeof(*(p_socket_t->addr_u.p_ipv6));
        p_socket_t->addr_u.p_ipv6->sin6_port = htons(p_socket_t->port);

        // Specify the pretended address
        if (p_socket_t->ip_address != NULL) {
            if (inet_pton(AF_INET6, p_socket_t->ip_address, &(p_socket_t->addr_u.p_ipv6->sin6_addr)) <= 0) {
                perror("Error, invalid address");
                return -1;
            }
        }
        else if (p_socket_t->is_server) {
            p_socket_t->addr_u.p_ipv6->sin6_addr = in6addr_any; 
        }
        else {
            ERROR_VERBOSE_LOG("Error, invalid address for the client to connect");
        }

        // Define the socket as DUAL-STACK
        if (p_socket_t->is_server) {
            int opt = 0; // Option value to turn off IPV6_V6ONLY
            if (setsockopt(p_socket_t->sock_FD, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) < 0)
            {
                ERROR_VERBOSE_LOG("Error setting socket options");   
                return -1;
            }
        }
    }

    return 0;
}

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

UniSocket_t *allocate_socket_struct(bool is_ipv4_arg)
{
    UniSocket_t *p_socket_t = (UniSocket_t *)malloc(sizeof(UniSocket_t));
    if (p_socket_t == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for the struct");
        return NULL;
    }

    // Address
    if (is_ipv4_arg) { //TODO consider the struct used for the IP
        p_socket_t->addr_u.p_ipv4 = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
        if (p_socket_t->addr_u.p_ipv4 == NULL)
        {       
            free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
            ERROR_VERBOSE_LOG("Error allocating memory for the address variable, ipv4 version");
            return NULL;
        }
    }
    else {
        p_socket_t->addr_u.p_ipv6 = (struct sockaddr_in6 *)malloc(sizeof(struct sockaddr_in6));
        if (p_socket_t->addr_u.p_ipv6 == NULL)
        {       
            free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
            ERROR_VERBOSE_LOG("Error allocating memory for the address variable, ipv6 version");
            return NULL;
        }
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

UniSocket_t *create_socket_struct(bool is_server_arg, int port, bool is_ipv4_arg, char *ip_address)
{
    UniSocket_t *p_socket_t;
    if ((p_socket_t = allocate_socket_struct(is_ipv4_arg)) == NULL)
    {
        ERROR_VERBOSE_LOG("Error allocating memory for the socket struct");
        return NULL;
    }

    // GENERAL settings (just storing the arguments, basically)
    p_socket_t->is_server = is_server_arg;
    p_socket_t->port = port;
    p_socket_t->is_ipv4 = is_ipv4_arg;
    p_socket_t->ip_address = (char *)malloc(MAX_ADDRESS_LENGTH);
    strncpy(p_socket_t->ip_address, ip_address, strlen(ip_address));

    if (assign_descriptor_to_stream_socket_t(p_socket_t) < 0)
    {
        ERROR_VERBOSE_LOG("Error assigning descriptor to the socket");
        free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
        return NULL;
    }
    
    if (initialize_socket(p_socket_t) < 0)
    {
        ERROR_VERBOSE_LOG("Error initializing the socket");
        free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t);
        return NULL;
    }

    // Start specific action for the socket 
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
    else {
        if (connect_to_server(p_socket_t) < 0)
        {
            ERROR_VERBOSE_LOG("Error connecting the client socket to the server socket");
            free_server_socket_memory_with_ptr_to_ptr((void **)&p_socket_t); // useful for the client too
            return NULL;
        }
    }

    return p_socket_t;
}
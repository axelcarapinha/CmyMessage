#include "net_utils_tcp.h"

int fill_buffer_with_response(ClientInfo_t *p_client_t) {
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    ssize_t bytes_received;
    if ((bytes_received = recv(p_client_t->sock_FD, p_client_t->buffer, BUFFER_SIZE, 0)) < 0)
    {
        ERROR_VERBOSE_LOG("Error receiving the preferred name from the client");
        return -1;
    }
    else if (bytes_received == 0)
    {
        printf("Client terminated the connection.\n");
        return -2; 
    }

    p_client_t->buffer[bytes_received] = '\0'; //TODO: check if the change in -1 does not make it work worst

    return bytes_received; //TODO cuidado com isto, caso deia erro
}

int is_ipv4(char *address) {
    struct in_addr ipv4_addr;
    int result = inet_pton(AF_INET, address, &ipv4_addr); // tries the conversion
    return result == 1; // 1 if it's IPv4
}

void *prepare_client_structs_for_data(ClientInfo_t *p_client_t)
{
    if (p_client_t == NULL)
    {
        ERROR_VERBOSE_LOG("Received a pointer pointing to improperly allocated memory");
        return NULL;
    }
    //
    p_client_t->name = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (p_client_t->name == NULL)
    {
        ERROR_VERBOSE_LOG("Error alocating memory for the p_client_t struct name");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return NULL;
    }

    p_client_t->buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (p_client_t->buffer == NULL)
    {
        ERROR_VERBOSE_LOG("Error alocating memory for the buffer pointer");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return NULL;
    }

    // Get the client's IP address (for a unique identification)
    // char ip_buffer[INET_ADDRSTRLEN]; //TODO use for IPv6
    // if (inet_ntop(AF_INET6, (void *)&(p_client_t->p_addr), ip_buffer, INET_ADDRSTRLEN) == NULL) //TODO handle IPV6 too
    // {
    //     ERROR_VERBOSE_LOG("Error converting IP address");
    //     free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
    //     return NULL;
    // }

    // p_client_t->addr_info = ip_buffer;
}

int use_service(int server_port, char *server_ip, ServiceFunctionPtr p_service_func) {
    
    // In this case (the client) the port will be the port 
    // that the client wants to connect to
    UniSocket_t *p_socket_t = create_socket_struct(false, server_port, is_ipv4(server_ip), server_ip);
    if (p_socket_t == NULL) {
        perror("Error creating socket for the client for the desired service");
        return -1;
    }
    INFO_VERBOSE_LOG("Client socket created successfully\n");
    
    // Prepare the client for the service section
    ClientInfo_t *p_client_t = (ClientInfo_t *)malloc(sizeof(ClientInfo_t));
    prepare_client_structs_for_data(p_client_t);
    if (p_client_t == NULL) {
        perror("Error initializing the struct for the client information");
        return -1;
    }

    // Assign the important information from the created socket
    p_client_t->sock_FD = p_socket_t->sock_FD;

    // Redirect the client to the CLIENT-SIDE service function
    (*p_service_func)(p_client_t);

    close_socket_with_ptr_if_open(&p_socket_t->sock_FD);

    return 0;
}
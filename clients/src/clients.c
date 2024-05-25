#include "net_utils_tcp.h"

//! The purpose of this file is generalize some functions of the client side part of services

//----------------------------------------------------------------------------------------------------------
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
        return -2; //TODO generalize this value in another define value
    }

    p_client_t->buffer[bytes_received] = '\0'; //TODO: check if the change in -1 does not make it work worst

    return bytes_received; //TODO cuidado com isto, caso deia error
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
//TODO consider using this in a library
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


//----------------------------------------------------------------------------------------------------------
/**
 * @brief 
 * 
 * @return int 
 */
int use_service(int server_port, char *server_ip, ServiceFunctionPtr p_service_func) {

    //TODO determine the type of socket to create from the address
    
    // In this case (the client) the port will be the port 
    // that the client wants to connect to
    UniSocket_t *p_socket_t = create_socket_struct(false, server_port, false, server_ip); //TODO: change for IPv6
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
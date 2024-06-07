/*
 * Purpose: Abstract the creation and handling of 
 * code regarding sockets, unifying it all in a common 
 * structure 
 */

#ifndef NET_UTILS_TCP_H
#define NET_UTILS_TCP_H

#include <sys/select.h>
#include "hash_table.h"
#include "queue.h"

//! Consider reading the description in the header file,
//! for the recommended choice of protocol when creating the socket

typedef struct
{
    int sock_FD;
    bool is_server;
    bool is_ipv4;
    union
    {
        struct sockaddr_in *p_ipv4;
        struct sockaddr_in6 *p_ipv6;
    } addr_u;
    socklen_t *p_addr_len;
    uint16_t port;
    char *ip_address;

    ServiceFunctionPtr p_service_func;

    // Thread related variables
    pthread_t *p_thread_pool;
    pthread_mutex_t *p_mutex_queue;
    pthread_cond_t *p_condition_var;
    //
    pthread_mutex_t *p_mutex_quit_signal;
    volatile sig_atomic_t *p_quit_signal;
    //
    pthread_mutex_t *p_mutex_usernames_ht;
    hash_table *p_usernames_ht; //! Uncomment in case it's a server!
    //
    fd_set *p_online_clients_set;
    pthread_mutex_t *p_mutex_online_clients_set;
    int *p_max_socket_so_far;
    pthread_mutex_t *p_mutex_common_msg_buffer;
    char *p_common_msg_buffer;
} UniSocket_t;


/**
 * @brief Marks the sockets as closed, 
 * to avoid closing more than once
 */
void close_socket_with_ptr_if_open(int *p_socket_descriptor);

/**
 * @brief Gracefully closes the server sockets
 */
void close_server_socket(UniSocket_t *p_socket_t);

/**
 * @brief Handles segmentation faults (SIGSEGV)
 * that may occur due to double-freeing memory,
 * avoiding a complet crash of the server
 */
void sigsegv_handler(int signum);

/**
 * @brief Fully frees the memory of the server (socket struct),
 * if it was NOT done previously
 * (can be seen as a destructor, like in C++)
 */
void free_server_socket_memory_with_ptr_to_ptr(void **p_p_socket_t_arg);

/**
 * @brief Frees the memory of the client, 
 * if it was NOT done previously
 * (can be seen as a destructor, like in C++)
 * 
 */
void free_client_memory_with_ptr_to_ptr(void **p_p_client_t_arg);

/**
 * @brief Connects a (client) socket to the server's socket
 * with the abstraction of the UniSocket_t struct
 * 
 * @return int (exit status)
 */
int connect_to_server(UniSocket_t* p_socket_t);

/**
 * @brief Accepts an incoming connection,
 * and stores the descriptor of it
 * 
 * @return ClientInfo_t* (pointer to the struct containing the 
 * client information: it's socket, ...)
 */
ClientInfo_t *accept_connection(int service_FD);

/**
 * @brief Attaches the (service) socket to the pretended port,
 * binds it to the pretended ip address
 * and puts it listening for incoming connections
 * 
 * @return int (exit status)
 */
int setup_service_socket_t(int opt, UniSocket_t *p_socket_t);

/**
 * @brief Initializes the socket with the pretended attributes
 * It's recommended to always use the DUAL-STACK socket option,
 * as it supports both IPv4 and IPv6 connections.
 * 
 * If the system or network configs do NOT support dual-stack sockets,
 * creating an IPv4 socket, while NOT recommended, 
 * allows the server to accept only IPv4 connections.
 * 
 * Noteworthy, if the server has IPv4 and IPv6 sockets,
 * please, consider changing opt to 1, for a more controlled flow of the connections
 * @return int (exit status)
 */
int initialize_socket(UniSocket_t *p_socket_t);

/**
 * @brief Defines the options for a STREAM socket,
 * assigning its corresponding descriptor at the end
 * 
 * @return int (exit status)
 */
int assign_descriptor_to_stream_socket_t(UniSocket_t *p_socket_t);

/**
 * @brief Allocates memory for the client struct
 * 
 * @return ClientInfo_t* (pointer to the struct of the client
 * and its corresponding informations)
 */
ClientInfo_t *allocate_client_info_struct();

/**
 * @brief Allocates memory for the needed elements of the entire
 * socket struct, depending on the type of used protocol (IPv4 or IPv6)
 * 
 * @return UniSocket_t* (pointer to the struct containing the socket's information)
 */
UniSocket_t *allocate_socket_struct(bool is_ipv4_arg);

/**
 * @brief Handles the entire creation of a STREAM socket
 * to handle connections for clients or servers
 * 
 * @return UniSocket_t* (pointer to the struct containing the socket's information)
 */
UniSocket_t *create_socket_struct(bool is_server_arg, int port, bool is_ipv4_arg, char *ip_address);


#endif

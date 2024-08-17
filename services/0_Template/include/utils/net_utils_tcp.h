#ifndef NET_UTILS_TCP_H
#define NET_UTILS_TCP_H

#include "clients.h" // clients, SETTINGS, ...
#include "hash_table.h"
#include "queue.h"

// If the system or network configs does NOT support dual-stack sockets,
// this approach, while NOT recommended, allows the server to accept only IPv4 connections.
// Noteworthy, if the server has IPv4 and IPv6 sockets,
// please, consider changing opt to 1, for a more controlled flow of the connections
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
    hash_table *p_usernames_ht;
    //
    fd_set *p_online_clients_set;
    pthread_mutex_t *p_mutex_online_clients_set;
    int *p_max_socket_so_far;
    pthread_mutex_t *p_mutex_common_msg_buffer;
    char *p_common_msg_buffer;
} UniSocket_t;


void close_socket_with_ptr_if_open(int *p_socket_descriptor);
void close_server_socket(UniSocket_t *p_socket_t);
void sigsegv_handler(int signum);
void free_server_socket_memory_with_ptr_to_ptr(void **p_p_socket_t_arg);
void free_client_memory_with_ptr_to_ptr(void **p_p_client_t_arg);
ClientInfo_t *allocate_client_info_struct();
ClientInfo_t *accept_connection(int service_FD);
int setup_service_socket_t(int opt, UniSocket_t *p_socket_t);
int assign_descriptor_to_stream_socket_t(UniSocket_t *p_socket_t);
int initialize_socket(UniSocket_t *p_socket_t);
UniSocket_t *allocate_socket_struct();
UniSocket_t *create_socket_struct(bool is_server_arg, int port, bool is_ipv4_arg);


#endif
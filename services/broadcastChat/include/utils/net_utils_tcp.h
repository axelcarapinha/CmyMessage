#ifndef NET_UTILS_TCP_H
#define NET_UTILS_TCP_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

// TODO verify unnecessary imports here
#include "clients.h"
#include "hash_table.h"

// Colors for stdout text
#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"

// Settings
#define SIZE_THREAD_POOL 3


#define VERBOSE true

// Function macros
#define ERROR_VERBOSE_LOG(string) \
    if (VERBOSE) { \
        perror(string); \
    } \

#define INFO_VERBOSE_LOG(string) \
    if (VERBOSE) { \
        printf(string); \
    } \


// If the system or network configs does NOT support dual-stack sockets,
// this approach, while NOT recommended, allows the server to accept only IPv4 connections.
// Noteworthy, if the server has IPv4 and IPv6 sockets,
// please, consider changing opt to 1, for a more controlled flow of the connections

//TODO fazer a struct no .c, para náo se conseguir mexer no .h
//TODO fazer set e get no .h (para ser mais elegante)

//TODO cuidado com o static (variável sempre o mesmo valor)

//TODO static para overflow vulnerabilidades
//TODO fazer uma struct dentro de uma struct (e fazer a declaração apenas no .c)
//TODO fazer o tyupedeef para ser reconhecida dentro de outra estrutura



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
    // TODO use in a separate struct (avoid many locks)
    fd_set *p_online_clients_set;
    pthread_mutex_t *p_mutex_online_clients_set;
    int *p_max_socket_so_far;
    pthread_mutex_t *p_mutex_common_msg_buffer;
    char *p_common_msg_buffer;
} UniSocket_t;
// TODO check the server closing function after all changes in the utils code

// Function prototypes
UniSocket_t *create_socket_struct(bool is_server_arg, int port, bool is_ipv4_arg);
UniSocket_t *allocate_socket_struct();
int initialize_socket(UniSocket_t *p_socket_t);
int assign_descriptor_to_stream_socket_t(UniSocket_t *p_socket_t);
int setup_service_socket_t(int opt, UniSocket_t *p_socket_t);
void close_server_socket(UniSocket_t *p_socket_t);

void free_server_socket_memory_with_ptr_to_ptr(void **p_p_socket_t);
//
ClientInfo_t *accept_connection(int service_FD);
ClientInfo_t *allocate_client_info_struct();
void ensure_client_disconnection_and_remove_data(ClientInfo_t *p_client_t);
void free_client_memory_with_ptr_to_ptr(void **p_p_client_t);
//
void close_socket_with_ptr_if_open(int *p_socket_descriptor);

#endif
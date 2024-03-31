#ifndef NET_UTILS_TCP_H
#define NET_UTILS_TCP_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
// TODO verify unnecessary imports here

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

// Forward-declared to avoid circular dependencies
typedef struct ClientInfo_t ClientInfo_t;
typedef int (*ServiceFunctionPtr)(ClientInfo_t *);

struct ClientInfo_t
{
    // Specifically ALLOCATED in the usages
    struct sockaddr *p_addr;
    socklen_t *p_addr_len;
    char *buffer;

    // Within the struct
    int sock_FD;
    char *addr_info;
    char *name;
    char *recipient;
    ServiceFunctionPtr p_service_func;
    //
    volatile sig_atomic_t *p_quit_signal;
    pthread_mutex_t *p_mutex_quit_signal;
    hash_table *p_usernames_ht;
    pthread_mutex_t *p_mutex_usernames_ht; // client's hash table
};

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

} UniSocket_t;
// TODO check the server closing function after all changes in the utils code

// Function prototypes
UniSocket_t *create_socket_struct(bool is_server_arg, int port, bool is_ipv4_arg);
UniSocket_t *allocate_socket_struct();
int initialize_socket(UniSocket_t *p_socket_t);
int assign_descriptor_to_stream_socket_t(UniSocket_t *p_socket_t);
int setup_service_socket_t(int opt, UniSocket_t *p_socket_t);
void close_server_socket(UniSocket_t *p_socket_t);

void free_unisocket_memory_with_ptr_to_ptr(void **p_p_socket_t);
//
ClientInfo_t *accept_connection(int service_FD);
ClientInfo_t *allocate_client_info_struct();
void ensure_client_disconnection_and_remove_data(ClientInfo_t *p_client_t);
void free_client_memory_with_ptr_to_ptr(void **p_p_client_t);
//
void close_socket_with_ptr_if_open(int *p_socket_descriptor);

#endif
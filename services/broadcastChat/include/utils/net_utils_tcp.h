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

typedef struct ClientInfo_t ClientInfo_t; // declared here to avoid circular dependency
typedef void (*ServiceFunctionPtr)(ClientInfo_t *);

struct ClientInfo_t
{
    // Specifically ALLOCATED in the usages
    struct sockaddr *p_addr;
    socklen_t *p_addr_len;
    char *buffer;

    // Within the struct
    long sock_FD;
    char *addr_info;
    char *name;
    char *recipient;
    ServiceFunctionPtr p_service_func;
};

// If the system or network configs does NOT support dual-stack sockets,
// this approach, while NOT recommended, allows the server to accept only IPv4 connections.
// Noteworthy, if the server has IPv4 and IPv6 sockets,
// please, consider changing opt to 1, for a more controlled flow of the connections
typedef struct
{
    int sock_fd;
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

} UniSocket_t;

// Functions
UniSocket_t *create_socket_struct(bool is_server_arg, int port, bool is_ipv4_arg);

#endif
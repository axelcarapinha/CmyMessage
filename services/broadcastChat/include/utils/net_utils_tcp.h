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

// Colors
#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"

#define SIZE_p_THREAD_POOL 3

//  Ensure compatibility with the service header file
typedef struct ClientInfo ClientInfo;

// Service function pointer
typedef void (*ServiceFunctionPtr)(ClientInfo *);

// Declared here to avoid circular dependency
struct ClientInfo
{
    long client_handler_FD;
    //
    struct sockaddr_in *p_ipv4_addr;
    socklen_t *p_addr_len;
    char *addr_info;
    //
    char *name;
    char *recipient;
    char *buffer;

    ServiceFunctionPtr p_service_func;
};

// Socket for IPV4 and IPV6 addresses for an easier code refactoring
// in case of needed support in older systems
// due to incompatibilities with IPV6_V6ONLY
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
UniSocket_t *create_socket(bool is_server_arg, int port, bool is_ipv4_arg);
int create_descriptor(UniSocket_t *p_socket_t);
void initialize(UniSocket_t *p_socket_t);
void setupServer(int opt, UniSocket_t *p_socket_t);
ClientInfo *acceptConnection(int echo_server);
void close_server_socket(UniSocket_t *p_socket_t);

#endif
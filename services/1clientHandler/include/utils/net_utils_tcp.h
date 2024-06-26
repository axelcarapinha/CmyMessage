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

// Colors
#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"


//TODO do in another way
// Ensure compatibility with the service header file
typedef void (*ServiceFunctionPtr)(void *);

// Socket for IPV4 and IPV6 addresses
typedef struct 
{
    int sock_fd;
    union
    {
        struct sockaddr_in *addr_ipv4;
        struct sockaddr_in6 *addr_ipv6;
    } address;
    socklen_t addrlen;
    uint16_t port;
    bool is_server;
    bool is_ipv4;
    ServiceFunctionPtr service_function_ptr;
} uniSocket;

// Functions
uniSocket *create_socket(bool is_server_arg, int port, bool is_ipv4_arg);
    int create_descriptor(uniSocket *socket_struct_ptr);
    void initialize(uniSocket *socket_struct_ptr);
    void setupServer(int opt, uniSocket *socket_struct_ptr);
int acceptConnection(int echo_server, struct sockaddr *address, socklen_t *addrlen);
void close_server_socket(uniSocket *socket_struct_ptr);

#endif
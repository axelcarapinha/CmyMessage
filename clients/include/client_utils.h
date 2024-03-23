#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <regex.h>
//
#define PORT 8040
#define CHAT_PORT 1300 
#define DAY_TIME_PORT 1320
//
#define BUFFER_SIZE 1500 // widely used in LANs (size of ethernet packets)
#define MAX_NUM_CONNECTIONS 10

#define ADDRESS "127.0.0.1"

// Text colors 
//TODO use the config file
#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"

enum ServerConnectionOptions
{
    LOGIN,
    REGISTER,
    LOGOUT,
    GUEST
};

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
} uniSocket;

// Utils
uniSocket *create_socket(bool is_server_arg, int port, bool is_ipv4_arg);
    int create_descriptor(uniSocket *socket_struct_ptr);
    void initialize(uniSocket *socket_struct_ptr);
int acceptConnection(int echo_server, struct sockaddr *address, socklen_t *addrlen);
void connect_cli(uniSocket *cli_struct_ptr, int port);
void close_socket(uniSocket *socket_struct_ptr);

#endif
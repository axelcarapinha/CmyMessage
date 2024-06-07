/*
 * Purpose: Implements auxiliar functions for services related to clients,
 * providing abstractions for content exchange 
 * between the server and the client
 */

#ifndef CLIENTS_H
#define CLIENTS_H

// Settings
#define VERBOSE true
//
#define SIZE_THREAD_POOL 3
#define BUFFER_SIZE 1500       // widely used in LANs (size of ethernet packets)
#define MAX_NUM_CLIENTS 2
#define MAX_USERNAME_LENGTH 20 // 20 chars, 20 bytes
#define USERNAMES_HASH_SEED 23
#define MAX_ADDRESS_LENGTH 128 // considering IPv6 addresses
//
#define FTP_COMMANDS_PORT 8021
#define FTP_DATA_PORT 8020
#define FTP_ADDR_IPV4 "127.0.0.1" 
#define FTP_ADDR_IPV6 "::1" 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <asm-generic/socket.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <signal.h>
#include <regex.h>

// Colors for stdout text
#define RESET "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"

// Function Macros and others
#define ERROR_VERBOSE_LOG(string) \
    if (VERBOSE) { \
        perror(string); \
    } \
//
#define INFO_VERBOSE_LOG(string) \
    if (VERBOSE) { \
        printf(string); \
    } \

// Forward-declared to avoid circular dependencies
typedef struct ClientInfo_t ClientInfo_t;
typedef int (*ServiceFunctionPtr)(ClientInfo_t *);

struct ClientInfo_t
{
    // Client information variables
    int sock_FD;
    //
    struct sockaddr *p_addr;
    socklen_t *p_addr_len;
    char *buffer;
    char *addr_info;
    char *name;
    char *recipient;

    // Service variables
    ServiceFunctionPtr p_service_func;

    // Thread control variables 
    pthread_mutex_t *p_mutex_quit_signal;
    pthread_mutex_t *p_mutex_usernames_ht; // client's hash table
    pthread_mutex_t *p_mutex_online_clients_set;
    pthread_mutex_t *p_mutex_common_msg_buffer;

    // For threads and other purposes
    volatile sig_atomic_t *p_quit_signal;
    void *p_usernames_ht;
    fd_set *p_online_clients_set;
    char *p_common_msg_buffer;
};

/**
 * @brief Receives the content (if available) on the client socket,
 * and identifies possible drops and errors of the connection
 * with the returning value of recv()
 *
 * @return int (number of bytes received on the client socket)
 */
int fill_buffer_with_response(ClientInfo_t *p_client_t);

/**
 * @brief Tries to convert the address to the IPv6 version, 
 * allowing to confirm it's version, 
 * and a propper socket creation in other parts of this code
 * 
 * @return 1 if the address is a valid IPv6 address, 0 otherwise
 */
int is_ipv4(char *address);

/**
 * @brief Allocates memory for the client struct, 
 * concerning the use of the pretende service
 * (avoiding to allocate more than needed when 
 * the corresponding socket is made, in net_utils_tcp.c)
 *
 * @return A pointer to the allocated UniSocket_t structure on success with allocation, otherwise NULL
 */
void *prepare_client_structs_for_data(ClientInfo_t *p_client_t);

/**
 * @brief Prompts the memory allocation needed for the client struct
 * redirects to the desired service 
 * when dereferencing the pointer of its starting function
 * 
 * @param p_service_func (pointer to the starting function of
 * the pretended service)
 *
 * @return int (exit status)
 */
int use_service(int server_port, char *server_ip, ServiceFunctionPtr p_service_func);


#endif
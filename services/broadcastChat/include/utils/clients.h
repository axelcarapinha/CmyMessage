#ifndef CLIENTS_H
#define CLIENTS_H

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <signal.h>

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
    void *p_usernames_ht;
    pthread_mutex_t *p_mutex_usernames_ht; // client's hash table
    //
    fd_set *p_online_clients_set;
    pthread_mutex_t *p_mutex_online_clients_set;
    char *p_common_msg_buffer;
};

#endif
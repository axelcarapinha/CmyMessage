#ifndef BROADCAST_CHAT_SERVICE_H
#define BROADCAST_CHAT_SERVICE_H

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

//TODO use the config file
// Default values
#define DEFAULT_PORT 8041
#define DEFAULT_BUFFER_SIZE 1500 // widely used in LANs (size of ethernet packets)
#define DEFAULT_MAX_NUM_CLIENTS 30

// Other values
#define SERVER_NAME "CmyMessage" // in case I end up changing the server name with updates
#define MAX_SIZE_USER_OPTION 1 // a number and the newline character

// Prepare the needed structs data for the service
typedef void (*ServiceFunctionPtr)(void *);
//
typedef struct
{
    int socket_cli;
    struct sockaddr_in client_addr;
    char *name;
    char *recipient;
} ClientInfo;

enum ServerConnectionOptions
{
    LOGIN,
    REGISTER,
    LOGOUT,
    GUEST,
    EXIT,
    INVALID_OPTION
};

// Functions
void join_client_to_broadcast_chat(long *client_handler_ptr, char *buffer);



#endif
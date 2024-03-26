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
#include <semaphore.h>
//
#include "net_utils_tcp.h"
#include "service_utils.h"
//TODO include just the necessary headers


// TODO use the config file
//  Default values
#define DEFAULT_PORT 8041
#define DEFAULT_BUFFER_SIZE 1500 // widely used in LANs (size of ethernet packets)
#define DEFAULT_MAX_NUM_CLIENTS 30
#define MAX_NUM_CLIENTS 2

// Other values
#define SERVER_NAME "CmyMessage" // in case I end up changing the server name with updates
#define MAX_SIZE_USER_OPTION 1   // a number and the newline character
#define MAX_SIZE_ADDR_INFO 200

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
void start_broadcasting_client_on_separate_thread(void *client_handler_ptr_arg);
    void prepare_client_structs_for_data_and_start_joining(void *client_handler_ptr_arg);
        void prepare_to_join_client_to_broadcast_chat(ClientInfo *client_struct_ptr);
        void join_client_to_broadcast_chat(long client_handler_FD, char *buffer, ClientInfo *client_struct_ptr);

#endif
#ifndef BROADCAST_CHAT_SERVICE_H
#define BROADCAST_CHAT_SERVICE_H

#include <semaphore.h>

//TODO solve the CMake file problem,
//TODO I am needing to import this here too for some reason
#include <pthread.h> 
//
#include "net_utils_tcp.h"
#include "service_utils.h"

// TODO use the config file
//  Default values
#define DEFAULT_PORT 8041
#define BUFFER_SIZE 1500 // widely used in LANs (size of ethernet packets)
#define DEFAULT_MAX_NUM_CLIENTS 30
#define MAX_NUM_CLIENTS 2

// Other values
#define SERVICE_NAME "BROADCAST CHAT" 
#define MAX_SIZE_USER_OPTION 1 // a single number (for the option)
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

// Function prototypes
int prepare_client_for_broadcast_and_start(ClientInfo_t *p_client_t);
    void * prepare_client_structs_for_data(ClientInfo_t *p_client_t);
    int prepare_to_broadcast_chat(ClientInfo_t *p_client_t);
        int broadcast_client(ClientInfo_t *p_client_t);

#endif
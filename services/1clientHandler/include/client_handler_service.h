#ifndef CLIENT_HANDLER_SERVICE_H
#define CLIENT_HANDLER_SERVICE_H

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
//
#include "net_utils_tcp.h"

// TODO use the config file
//  Default values
#define DEFAULT_PORT 8040
#define BROADCAST_CHAT_PORT 8050
#define DEFAULT_BUFFER_SIZE 1500 // widely used in LANs (size of ethernet packets)
#define DEFAULT_MAX_NUM_CLIENTS 30

// Other values
#define SERVER_NAME "CmyMessage" // in case I end up changing the server name with updates
#define MAX_SIZE_USER_OPTION 1   // a number and the newline character

// For the generalization of the service include file
typedef void (*ServiceFunctionPtr)(void *);

enum UserAuthOptions
{
    LOGIN,
    REGISTER,
    LOGOUT,
    GUEST,
    EXIT,
    INVALID_OPTION
};

enum DesiredServiceOptions
{
    BROADCAST_SERVICE,
    DAYTIME_SERVICE
};

// Functions
void handle_client_requests(void *client_handler_ptr_arg);
void introduce_options_to_the_client(long *client_handler_ptr, char *buffer);
void get_user_option_and_forward_to_desired_service(long *client_handler_ptr, char *buffer);
int get_user_selected_option(long *client_handler_ptr, char *buffer);

#endif
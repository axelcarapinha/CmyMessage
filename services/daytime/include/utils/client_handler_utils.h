#ifndef CLIENT_HANDLER_UTILS_H
#define CLIENT_HANDLER_UTILS_H

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
//TODO usar ficheiro de configs para estas coisas (mesmo do admin side)
#define BUFFER_SIZE 1500 // widely used in LANs (size of ethernet packets)

// Text colors
#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"

#define SERVER_NAME "CmyMessage" // in case I end up changing the server name with updates
#define MAX_SIZE_USER_OPTION 1 // a number and the newline character

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

// Utils
void handle_client_requests(void *client_handler_ptr_arg);
    void introduce_options_to_the_client(long *client_handler_ptr);
    void get_user_option_and_forward_to_desired_service(long *client_handler_ptr);
        int get_user_selected_option(long *client_handler_ptr);

// Authentication (will be in its own file)
void login_client(long *client_handler_ptr_arg, char *buffer);
void register_client(long *client_handler_ptr_arg, char *buffer);
void logout_client(long *client_handler_ptr_arg, char *buffer);
void enter_cli_as_guest(long *client_handler_ptr_arg, char *buffer);








#endif
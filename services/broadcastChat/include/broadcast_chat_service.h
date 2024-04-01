#ifndef BROADCAST_CHAT_SERVICE_H
#define BROADCAST_CHAT_SERVICE_H

#include "net_utils_tcp.h"
#include "service_utils.h"

//TODO solve the CMake file problem,
//TODO I am needing to import this here too for some reason
#include <pthread.h> 
#include <semaphore.h>

// TODO use the config file
//  Default values
#define DEFAULT_PORT 8041
#define MAX_NUM_CLIENTS 2
//
#define USERNAMES_HASH_SEED 23
#define MAX_USERNAME_LENGTH 20 // 20 chars, 20 bytes


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

//TODO mesma ordem para as funcs aqui e no .c
// Function prototypes
int prepare_client_for_broadcast_and_start(ClientInfo_t *p_client_t);
void *prepare_client_structs_for_data(ClientInfo_t *p_client_t);
int prepare_to_broadcast_chat(ClientInfo_t *p_client_t);
int get_messages_from_active_clients(ClientInfo_t *p_client_t);
void broadcast_message_to_online_clients(ClientInfo_t *p_client_t, long tot_num_bytes_recv);
void remove_socket_from_sets(ClientInfo_t *p_client_t, int socket_FD, fd_set ready_sockets_set);
void send_customized_welcome_message(ClientInfo_t *p_client_t);










#endif
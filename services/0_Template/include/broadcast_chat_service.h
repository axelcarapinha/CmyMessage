#ifndef BROADCAST_CHAT_SERVICE_H
#define BROADCAST_CHAT_SERVICE_H

#include <pthread.h>
#include <semaphore.h>
#include <asm-generic/socket.h>
//
#include "service_utils.h"

#define DEFAULT_PORT 8041
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

typedef struct
{
    pthread_mutex_t *p_mutex_broadcast_cond_var;
    pthread_cond_t *p_broadcast_condition_var;
    //
    pthread_mutex_t *p_mutex_working_bool;
    pthread_mutex_t *p_mutex_usernames_by_FD_arr;
    pthread_mutex_t *p_mutex_did_prepare_service_data;
    //
    bool is_broadcasting;
    bool did_prepare_service_data;
} ThreadControl_t;

typedef struct
{
    char **usernames_by_FD_arr; // easily map the name of the client to its file descriptor
    ThreadControl_t *p_threads_t;

} BroadcastControllers_t;

void wait_for_thread_work(BroadcastControllers_t *p_broadcast_ctrls_t_s);
int start_removing_client_from_broadcast(BroadcastControllers_t *p_broadcast_ctrls_t_s,
    ClientInfo_t *p_client_t, int socket_FD, fd_set ready_sockets_set);
int send_customized_welcome_message(ClientInfo_t *p_client_t);
int remove_socket_from_sets(ClientInfo_t *p_client_t, int socket_FD, fd_set ready_sockets_set);
int broadcast_message_to_online_clients(ClientInfo_t *p_client_t, long tot_num_bytes_recv);
int get_messages_from_active_clients(BroadcastControllers_t *p_broadcast_ctrls_t_s, ClientInfo_t *p_client_t);
int accept_and_store_client_username(BroadcastControllers_t *p_broadcast_ctrls_t_s, ClientInfo_t *p_client_t);
int ask_and_assign_username(BroadcastControllers_t *p_broadcast_ctrls_t_s, ClientInfo_t *p_client_t);
void *prepare_client_structs_for_data(ClientInfo_t *p_client_t);
ThreadControl_t *get_thread_control_struct_ptr();
BroadcastControllers_t *get_broadcast_controls_struct_ptr();
int prepare_client_for_broadcast_and_start(ClientInfo_t *p_client_t);

#endif
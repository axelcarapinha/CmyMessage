#ifndef FTP_SERVICE_H
#define FTP_SERVICE_H

#include <pthread.h>
#include <semaphore.h>
#include <asm-generic/socket.h>
//
#include "service_utils.h"
#include "clients.h"

// Settings
#define DEFAULT_PORT 8021
#define MAX_SIZE_USER_OPTION 1 // a single number (for the option)
#define MAX_SIZE_ADDR_INFO 200
//
#define SERVICE_NAME "FTP"

int serve_client_with_FTP(ClientInfo_t *p_client_t);
void *prepare_client_structs_for_data(ClientInfo_t *p_client_t);
int ask_client_basic_details(ClientInfo_t *p_client_t);
int send_customized_welcome_message(ClientInfo_t *p_client_t);
int define_acess_controls(ClientInfo_t *p_client_t);

int parse_client_command_and_redirect(int command);
int input_client_command_and_decode(ClientInfo_t *p_client_t);

int compress_file();
int write_file();
int send_file();
int open_file();












#endif
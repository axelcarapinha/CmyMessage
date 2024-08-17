#ifndef SERVICE_UTILS_H
#define SERVICE_UTILS_H

#include "net_utils_tcp.h"

#define CLOSE_SERVER 1

// Function prototypes
int start_service(int port, ServiceFunctionPtr p_server_t);
int start_accepting_incoming_connections(UniSocket_t *p_server_t);
hash_table *get_usernames_hash_table_ptr();
int listen_for_connections_on_separate_thread(UniSocket_t *p_server_t);
void initialize_server_concurrency_and_thread_pool(UniSocket_t *p_server_t);
void *accept_incoming_connections(void *p_server_t_arg);
void *search_for_thread_work(void *p_server_t_arg);
int join_thread_and_handle_errors(pthread_t *p_thread_ID);
void close_service(UniSocket_t *p_server_t);

#endif
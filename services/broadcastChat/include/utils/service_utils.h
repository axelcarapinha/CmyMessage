#ifndef SERVICE_UTILS_H
#define SERVICE_UTILS_H

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

// Functions
void start_service(int port, ServiceFunctionPtr service_function);
    void start_accepting_incoming_connections(uniSocket *server_struct_ptr);
        void accept_incoming_connections(void *server_struct_ptr_arg);
    void handle_thread_creation_and_exit(pthread_t *thread_id_ptr, int thread_create_status);
    void join_thread_and_handle_errors(pthread_t *thread_id_ptr);
void close_service(uniSocket *server_struct_ptr);

#endif
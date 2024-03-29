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
#include <signal.h>
//
#include "net_utils_tcp.h"
#include "queue.h"

#define CLOSE_SERVER 1

// Function prototypes
int start_service(int port, ServiceFunctionPtr p_server_t);
    int start_accepting_incoming_connections(UniSocket_t *p_server_t);
    void accept_incoming_connections(void *p_server_t_arg);
        void *search_for_thread_work(void *p_server_t_arg);
        int join_thread_and_handle_errors(pthread_t *p_thread_ID);
void close_service(UniSocket_t *p_server_t);

#endif
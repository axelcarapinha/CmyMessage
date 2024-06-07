#ifndef SERVICE_UTILS_H
#define SERVICE_UTILS_H

#include "net_utils_tcp.h"

#define CLOSE_SERVER 1

// Function prototypes
void close_service(UniSocket_t *p_server_t);



hash_table *get_usernames_hash_table_ptr();
void initialize_server_concurrency_and_thread_pool(UniSocket_t *p_server_t);
void *accept_incoming_connections(void *p_server_t_arg);
void *search_for_thread_work(void *p_server_t_arg);
int join_thread_and_handle_errors(pthread_t *p_thread_ID);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
int listen_for_connections_on_separate_thread(UniSocket_t *p_server_t);

/**
 * @brief

 *
 * @param
 * @param
 *
 * @return
 */
int start_accepting_incoming_connections(UniSocket_t *p_server_t);

/**
 * @brief Starts the service
 *
 * This function starts the service on the specified port by creating a server socket,
 * assigning the service function, and initiating the process of accepting incoming connections.
 * It handles any errors that may occur during the creation of the socket or the start of accepting connections.
 * After the service has been started, it calls close_service to clean up resources when the service ends
 * or is requested to end.
 *
 * @param port The port number for the service
 * @param p_service_func_arg Pointer to the function that implements the service functionality
 *
 * @return 0 on success, or a negative value indicating an error
 */
int start_service(int port, ServiceFunctionPtr p_server_t);

#endif
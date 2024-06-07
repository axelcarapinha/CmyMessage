#ifndef SERVICE_UTILS_H
#define SERVICE_UTILS_H

#include "net_utils_tcp.h"

#define VERBOSE true

#define CLOSE_SERVER 1
#define MAX_ADDRESS_LENGTH (32 + 7) // considering IPv6 addresses, the longest
#define BUFFER_SIZE 1500   // widely used in LANs (size of ethernet packets)
#define MAX_NUM_CLIENTS 30 // for the queue's size


/**
 * @brief Fills the buffer with the response 
 * from the CLIENT, in this case
 * 
 * @param p_client_t 
 * @return int 
 */
int fill_buffer_with_response(ClientInfo_t *p_client_t);

/**
 * @brief Gracefully closes the service, 
 * 
 * @param p_server_t 
 */
void close_service(UniSocket_t *p_server_t);

/**
 * @brief Autoexplanatory, I guess
 * @param p_thread_ID 
 * @return int 
 */
int join_thread_and_handle_errors(pthread_t *p_thread_ID);

/**
 * @brief It's where the "worker threads" from the thread pool
 * search for more clients in the queue concurrently,
 * and forward them to the desired service function
 * 
 * @param p_server_t_arg 
 * @return void* 
 */
void *search_for_thread_work(void *p_server_t_arg);

/**
 * @brief Sends messages to the clients
 * notifying about the termination of the service,
 * caused by a quit signal 
 * that tells all the worker threads
 * to NOT accept more clients from the queue
 * 
 * Can be triggered by the close_service() function,
 * in the end of the start_service()
 * @param p_client_t 
 * @param p_queue_mutex 
 */
void gracefully_reject_pending_clients(ClientInfo_t *p_client_t, pthread_mutex_t *p_queue_mutex);


/**
 * @brief Where the listening thread executes
 * the majority of the time,
 * accepting connections, inserting in the queue of requests,
 * and signaling the "worker threads",
 * 
 * @param p_server_t_arg 
 * @return void* 
 */
void *accept_incoming_connections(void *p_server_t_arg);

/**
 * @brief Allocate and initializes 
 * all the synchronization primitives
 * and assigns their pointers to the respective 
 * members of the server struct
 * 
 * @param p_server_t 
 */
void initialize_server_concurrency_and_thread_pool(UniSocket_t *p_server_t);

/**
 * @brief Allocates the usernames' hash table 
 * with the murmur3_32 has function as default
 * 
 * @return hash_table* (pointer to the allocated hash table)
 */
hash_table *get_usernames_hash_table_ptr();

/**
 * @brief Allocate space for a set of socket (file) descriptors,
 * 
 * @return fd_set* (pointer to the allocated set)
 */
fd_set *get_set_of_clients_descriptors();

/**
 * @brief Separate thread solely for listening for incoming connections,
 * allowing the server to be concurrent
 * 
 * @param p_server_t 
 * @return int (exit status)
 */
int listen_for_connections_on_separate_thread(UniSocket_t *p_server_t);

/**
 * @brief Prepares the server to handle the client connections,
 * by initializing the thread pool 
 * and listening for the connections on a unique separate thread
 * 
 * That listening thread will then redirect the users to the
 * "worker threads", each of the thread pool
 *
 * @param p_server_t Pointer to the server struct (containing info for the server)
 *
 * @return int (exit status)
 */
int start_accepting_incoming_connections(UniSocket_t *p_server_t);

/**
 * @brief Starts the desired service 
 * by dereferencing the corresponding function pointer
 *
 * @param port The port number for the service
 * @param p_service_func_arg Pointer to the function that implements the service functionality
 *
 * @return int (exit status)
 */
int start_service(int server_port, char *server_ip, ServiceFunctionPtr p_service_func_arg);


#endif
#include "net_utils_tcp.h"
#include "client_handler_utils.h"

#define PORT 8040
#define MAX_NUM_CLIENTS 30


void handle_thread_creation_and_exit(pthread_t *thread_id_ptr, int thread_create_status)
{
    if (thread_create_status != 0)
    {
        fprintf(stderr, "Value return from the thread creation is %d\n", thread_create_status);
        return;
    }

    // Checking for THREAD JOINING errors
    void *status_ptr; // pthread_join already allocates memory for the pointer
    int join_status = pthread_join(*thread_id_ptr, &status_ptr);
    if (join_status != 0)
    {
        fprintf(stderr, "Error joining listener thread: %d\n", join_status);
        return;
    }

    // Checking for EXECUTION errors
    if (status_ptr != NULL)
    {
        int *status_val = (int *)status_ptr;
        if (*status_val != 0)
        {
            fprintf(stderr, "Listener thread returned value %d\n", *status_val);
        }
    }

    free(status_ptr);
}

// TODO for receiving too
//  void send_lots_of_data_to_client() {

// }

// TODO close the sockets from the clients at the end
void accept_incoming_connections(void *server_struct_ptr_arg)
{
    uniSocket *server_struct_ptr = (uniSocket *)server_struct_ptr_arg;
    while (true)
    {
        long *client_handler_ptr = (long *)malloc(sizeof(long));
        if (client_handler_ptr == NULL)
        {
            perror("Problem allocating memory for the client_handler_ptr");
            exit(EXIT_FAILURE);
        }

        *client_handler_ptr = acceptConnection(server_struct_ptr->sock_fd,
                                               (struct sockaddr *)server_struct_ptr->address.addr_ipv4,
                                               &(server_struct_ptr->addrlen));
        if (*client_handler_ptr < 0)
        {
            perror("Error accepting connection");
            free(client_handler_ptr);
            continue;
        }

        // Handle requests in a separate thread
        pthread_t handler_thread;
        int thread_creation_status = pthread_create(&handler_thread, NULL, (void *(*)(void *))handle_client_requests, (void *)client_handler_ptr);
        // "void *" generic pointer
        // "(*)" function pointer
        // "(void *)" arguments types of the function

        handle_thread_creation_and_exit(&handler_thread, thread_creation_status);
    }
}

//* In the end, the MAX_NUMBER_OF_CONNECTIONS handles the amount of threading
void start_accepting_incoming_connections(uniSocket *server_struct_ptr)
{
    pthread_t listening_thread;
    int thread_creation_status;
    thread_creation_status = pthread_create(&listening_thread, NULL,
                                            (void *(*)(void *))accept_incoming_connections, (void *)server_struct_ptr);

    handle_thread_creation_and_exit(&listening_thread, thread_creation_status);
}

void start_server(int port)
{
    uniSocket *server_struct_ptr = create_socket(true, port, true);
    start_accepting_incoming_connections(server_struct_ptr);
    close_server_socket(server_struct_ptr);
}

int main(int argc, char *argv[])
{
    puts(YELLOW "Powering up the server!" RESET);
    start_server(PORT);

    // int status;
    // if ((status = start_server(PORT)) < 0) {
    //     fprintf(stderr, "Error number %d when starting the server.\n", status);
    // }

    return 0;
}
#include "net_utils_tcp.h"

#define PORT 8088

void start_server() {
    sleep(3);

    puts("Hello from the server");

    uniSocket *server_struct_ptr = create_socket(true, PORT, true);

    for (;;) {
        int client_socket = acceptConnection(server_struct_ptr->sock_fd,
            server_struct_ptr->address.addr_ipv4, 
            &(server_struct_ptr->addrlen));
        
        if (client_socket >= 0) {
            daytime_service(); //TODO importar o servidor corretamente
        }
        else {
            perror("Error connecting to the client");
            exit(EXIT_FAILURE);
        }
    }

    // void close_socket(server_struct_ptr);
    puts("Closed");
}
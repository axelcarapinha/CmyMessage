#include "net_utils_tcp.h"

void daytime_service(int talking_socket)
{
    char buffer[BUFFER_SIZE] = {0};
    bzero(buffer, BUFFER_SIZE); // clean garbage data

    // Calc the current time
    time_t curTime;
    struct tm *time_info;
    time(&curTime);
    time_info = localtime(&curTime);

    // Send the content
    strftime(buffer, BUFFER_SIZE, "Today is %d-%m-%Y %H:%M:%S", time_info);
    char *extra_sentence = "\nHave a nice day :)";
    strcat(buffer, extra_sentence);
    //
    send(talking_socket, buffer, strlen(buffer) + strlen(extra_sentence), 0);
    printf("Sent today's time '%s'\n", buffer);

    // FULLY terminate the connection
    bzero(buffer, BUFFER_SIZE);
    close(talking_socket);
}



//   // Prepare for the assync I/O operations
//     fd_set current_sockets, ready_sockets; //TODO add to the server
//     FD_ZERO(&current_sockets);
//     FD_SET(server_struct_ptr->sock_fd, &current_sockets);




//    // Monitor the sockets (file descriptors)
//         ready_sockets = current_sockets;
//         if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
//         {
//             fprintf(stderr, "Problem polling");
//             exit(EXIT_FAILURE);
//         }
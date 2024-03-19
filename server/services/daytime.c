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
#include "net_utils_tcp.h"

void echo_service(int talking_socket)
{
    char buffer[BUFFER_SIZE] = {0};
    bzero(buffer, BUFFER_SIZE);

    // Read the message, and echo the message
    const char *ECHO_WORD = "ECHO";
    char sendingBuffer[BUFFER_SIZE];
    strcpy(sendingBuffer, ECHO_WORD);
    //
    ssize_t valread;
    while ((valread = recv(talking_socket, buffer, BUFFER_SIZE - 1, 0)) > 0 &&
           strcmp(buffer, "exit") != 0)
    {

        // Add "ECHO" to the beggining of the word
        strcat(sendingBuffer, buffer);
        send(talking_socket, sendingBuffer, valread, 0);
        //
        buffer[strlen(buffer) - 1] = '\0';
        printf("Echoed the expression '%s'\n", buffer);
        //
        sendingBuffer[strlen(ECHO_WORD)] = '\0';
        buffer[valread] = '\0';

        // TODO se ainda há conteúdo por enviar, fazer mais
    }
    // >= do que zero no recv faz com que o loop continue mesmo quando não se tem nada mais para ler

    // FULLY terminate the connection
    bzero(buffer, BUFFER_SIZE);
    close(talking_socket);
}
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pthread_t thread_server;
    int iret;

    // Create a thread for the server
    iret = pthread_create(&thread_server, NULL, start_server, NULL);
    if (iret)
    {
        perror("Error creating the thread");
        exit(EXIT_FAILURE);
    }

    // Connect the client (after waiting)
    pthread_join(thread_server, NULL);
    connect_cli();

    return 0;
}
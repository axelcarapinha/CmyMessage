#include "client_utils.h"

/**
 * @brief
 *
 * @param a
 */
void chat(uniSocket *cli_struct_ptr)
{
    puts("Chat started. Print 'exit' to terminate the session.");

    // Prepare the buffer
    char *buffer = (char *)malloc(BUFFER_SIZE);

    // Prepare the regex expression
    regex_t regex;
    if (regcomp(&regex, "([eE][xX][iI][tT])", REG_EXTENDED) != 0) {
        fprintf(stderr, "Failed to compile regex\n");
        exit(EXIT_FAILURE);
    }
    //TODO consider using fprintf(stderr) instead of perror in more places
    //TODO of this code

    // Read the message
    ssize_t valread;
    while ((valread = recv(cli_struct_ptr->sock_fd, buffer, BUFFER_SIZE - 1, 0)) > 0 &&
           regexec(&regex, buffer, 0, NULL, 0) != 0)
    {
        // Print the received
        printf("%s\n", buffer);

        // Ask for more
        printf("> ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer)] = '\0'; // precaution

        // Send
        send(cli_struct_ptr->sock_fd, buffer, valread, 0);
        printf("%s\n", buffer);

        // TODO se ainda há conteúdo por enviar, fazer mais
    }

    bzero(buffer, BUFFER_SIZE);
    free(buffer);
    close(cli_struct_ptr->sock_fd);
}

int main(int argc, char *argv[])
{
    // Socket creation
    uniSocket *cli_struct_ptr = create_socket(false, PORT, true);

    // Client handler
    // check_cli_access_options(cli_struct_ptr);

    // Start the pretended service
    chat(cli_struct_ptr);

    // Close the socket
    close(cli_struct_ptr->sock_fd);

    return 0;
}
#include "client_utils.h"

/**
 * @brief
 *
 * @param a
 */
void chat(uniSocket *cli_struct_ptr)
{
    // Prepare the buffer
    char *buffer = (char *)malloc(BUFFER_SIZE);

    // Compile regex expression
    regex_t exit_regex;
    if (regcomp(&exit_regex, "([eE][xX][iI][tT])", REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Failed to compile regex\n");
        exit(EXIT_FAILURE);
    }

    printf("> ");
    fgets(buffer, BUFFER_SIZE, stdin);

    // Read the message
    while (true)
    {
        ssize_t valread;
        valread = recv(cli_struct_ptr->sock_fd, buffer, BUFFER_SIZE, 0);
        if (valread == 0) {
            printf("Could NOT read any content from the server");
        }

        // Print the received
        printf(BLUE "%s\n" RESET, buffer);
        memset(buffer, 0, BUFFER_SIZE);

        // Send response
        printf("> ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0';
        //
        if (regexec(&exit_regex, buffer, 0, NULL, 0) == 0) {
            printf("Exiting chat.\n");
            break; 
        }
        //
        send(cli_struct_ptr->sock_fd, buffer, strlen(buffer), 0);
        printf("Sent %d\n", strlen(buffer));

        // if valread
        // regexec(&regex, buffer, 0, NULL, 0) != 0)
        // TODO se ainda há conteúdo por enviar, fazer mais

        // Fully clean the buffer
        memset(buffer, 0, BUFFER_SIZE);
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
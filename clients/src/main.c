#include "client_utils.h"

void chat(uniSocket *cli_struct_ptr)
{
    char *buffer = (char *)malloc(BUFFER_SIZE);

    // Compile regex expression
    regex_t exit_regex;
    if (regcomp(&exit_regex, "([eE][xX][iI][tT])", REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Failed to compile regex\n");
        exit(EXIT_FAILURE);
    }

    //TODO func to read all content frmo the seber

    // Start sending content to the server
    printf("Starting chat... (press Ctrl-C to exit)\n");
    while (true)
    {
        ssize_t valread;
        valread = recv(cli_struct_ptr->sock_fd, buffer, BUFFER_SIZE, 0);
        if (valread < 0) {
            perror("Failed to received data from the server");
            close_socket(cli_struct_ptr);
            exit(EXIT_FAILURE);
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
        printf("Sent %ld\n", strlen(buffer));

        // if valread
        // regexec(&regex, buffer, 0, NULL, 0) != 0)
        // TODO se ainda há conteúdo por enviar, fazer mais

        // Fully clean the buffer
        memset(buffer, 0, BUFFER_SIZE);
    }

    // Terminate gracefully
    bzero(buffer, BUFFER_SIZE);
    free(buffer);
    close(cli_struct_ptr->sock_fd);
}

int main(int argc, char *argv[])
{
    uniSocket *cli_struct_ptr = create_socket(false, PORT, true);
    connect_cli(cli_struct_ptr, PORT);

    chat(cli_struct_ptr);
    // close(cli_struct_ptr->sock_fd);

    return 0;
}
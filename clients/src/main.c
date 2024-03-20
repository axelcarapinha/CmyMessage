#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string.h>
#include <stdbool.h>
#include "client_utils.h"

#define PORT 8088
#define ADDRESS "127.0.0.1"
#define BUFFER_SIZE 1024

#define PASSWORD_SIZE 150
#define MAX_USERNAME_SIZE 150

// RFC 5321 and RFC 5322 (respecting that standards)
// 64 (local part) + 1 (@) +255 (domain part)
#define MAX_EMAIL_SIZE 320

#define MAX_INPUT_SIZE 1 // just for a number //TODO overflow cuidado

enum Options
{
    REGISTER,
    LOGIN,
    LOGOUT
};

// void register()
// {
//     char email[MAX_EMAIL_SIZE];
//     char username[MAX_USERNAME_SIZE];

//     puts("Ok. Let's create an account...");
//     printf("Email: ");
//     fgets(email, MAX_EMAIL_SIZE + 1, stdin);
//     //TODO check for validity of the input

//     printf("Username: ");
//     fgets(username, MAX_USERNAME_SIZE + 1, stdin);
//     //TODO check for the validity of the input
// }

// void login_with_credentials()
// {
//     char user_input[];
//     fgets("");
// }

// void logout()
// {
//     char user_input[]
//     printf("Are you sure? [y / n] ");
//     fgets(username, MAX_INPUT_SIZE + 1, stdin);
// }

void interface()
{
    int option;
    char user_input[1];
    bool invalid_input = false;
    do
    {
        puts("--------------------Welcome to CmyMessage!--------------------");
        puts("(1) Register");
        puts("(2) Login");
        puts("(3) Logout");
        printf("Select the option: ");

        fgets(user_input, MAX_INPUT_SIZE + 1, stdin); // include the newline
        option = atoi(user_input);

        // Check if the option is valid
        switch (option)
        {
        case REGISTER:
            // register();
            break;
        case LOGIN:
            // login();
            break;
        case LOGOUT:
            // logout();
            break;
        default:
            perror("Invalid option, please, try again");
            invalid_input = true;
            break;
        }
    } while (invalid_input);

    puts("Did it heheh");
}

int main(int argc, char *argv[])
{
    puts("Hello from main");
    // interface();

    connect_cli();

    return 0;
}

void chat(uniSocket *cli_struct_ptr)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    bytes_received = recv(cli_struct_ptr->sock_fd, buffer, BUFFER_SIZE, 0);

    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0';
    }
    else if (bytes_received == 0)
    {
        puts("Connection terminated by the server.");
    }
    else {
        perror("Failed to receive the data");
    }

    printf("%s\n", buffer);

    return 0;

    // Read the content
    // if (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
    // {
    //     // Input sanitization (from the client)
    //     buffer[strcspn(buffer, "\n")] = '\0';
    //     puts("Text read successfully");
    // }
    // else
    // {
    //     perror("Reading input failed");
    //     puts("Please, try again");
    // }
    // TODO voltar a conectar após conseguir

    // Write the message

    // Send the message

    // Wait for response from the server
}

void connect_cli()
{
    // Socket creation
    uniSocket *socket_cli = create_socket(false, PORT, true);

    // Address settings
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    //
    if (inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) <= 0)
    {
        perror("Unsupported address.");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    int status;
    if ((status = connect(socket_cli->sock_fd,
                          socket_cli->address.addr_ipv4, socket_cli->addrlen)) >= 0)
    {
        chat(socket_cli);
    }
    else
    {
        perror("Connecting to the server failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

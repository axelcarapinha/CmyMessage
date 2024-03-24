#include "client_handler_utils.h"
// #include "services.h"

void login_client(long *client_handler_ptr, char *buffer)
{
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, "Username: ");
    send(*client_handler_ptr, buffer, BUFFER_SIZE, 0);

    // send('\n')
}

void register_client(long *client_handler_ptr, char *buffer)
{
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, "Pretended Username: ");
    send(*client_handler_ptr, buffer, BUFFER_SIZE, 0);
}

void logout_client(long *client_handler_ptr, char *buffer)
{
    // ClientInfo *recipient = (ClientInfo *)malloc(sizeof(ClientInfo));

    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, "You logged out. Have a nice day :)\n");
    send(*client_handler_ptr, buffer, BUFFER_SIZE, 0);
}

void enter_cli_as_guest(long *client_handler_ptr, char *buffer)
{
    ClientInfo *recipient = (ClientInfo *)malloc(sizeof(ClientInfo));
    memset(buffer, 0, BUFFER_SIZE);

    // Ask for a simple ID
    strcpy(buffer, "\nOur newest guest! How we can call you: ");
    send(*client_handler_ptr, buffer, strlen(buffer), 0);

    // Assign its ID
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received;
    if ((bytes_received = recv(*client_handler_ptr, buffer, BUFFER_SIZE, 0)) < 0)
    {
        fprintf(stderr, "recv() function failed, with exit code %d\n", bytes_received);
        exit(EXIT_FAILURE);
    }
    else if (bytes_received == 0)
    {
        fprintf(stderr, "Client terminated the connection.\n");
        exit(EXIT_SUCCESS);
    }

    // Assign the username to its struct (temporary memory)
    buffer[bytes_received] = '\0';
    strcpy(recipient->name, buffer);

    char *message = "Welcome to the broadcast channel, ";
    strcat(message, buffer);
    send(*client_handler_ptr, message, strlen(message), 0);
    while (true)
    {
        // Ask for input
        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, YELLOW "> " RESET);
        send(*client_handler_ptr, buffer, strlen(buffer), 0);
    }

    // Free allocated memory
    free(recipient);
}

// Enjoy the broadcast session

// TODO consider the block of processes
int get_user_selected_option(long *client_handler_ptr)
{
    int user_selected_option;

    // Send the prompting line
    char *buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    strcpy(buffer, "Enter the number of the desired option: ");
    send(*client_handler_ptr, buffer, strlen(buffer), 0);

    // Get input
    memset(buffer, 0, BUFFER_SIZE);
    size_t bytes_received;
    if ((bytes_received = recv(*client_handler_ptr, buffer, BUFFER_SIZE, 0)) < 0)
    {
        fprintf(stderr, "recv() method exited with error %d\n", (int)bytes_received);
        exit(EXIT_FAILURE);
    }

    // SANITIZE the input
    // buffer[bytes_received - 1] = '\0';

    // buffer[0] = bytes_received;
    // send(*client_handler_ptr, buffer, BUFFER_SIZE, 0);
    printf("Received = %ld\n", bytes_received);
    printf("Buffer = %s\n", buffer);

    if (isdigit(buffer[0]) && buffer[MAX_SIZE_USER_OPTION] == '\n')
    {
        buffer[MAX_SIZE_USER_OPTION] = '\0';
        int user_input = atoi(buffer);
        memset(buffer, bytes_received, 0);
        //
        int LAST_VALID_OPTION = INVALID_OPTION - 1;
        if (user_input > LAST_VALID_OPTION)
        {
            user_selected_option = INVALID_OPTION;
        }
        else
        {
            user_selected_option = user_input;
        }
    }
    else
    {
        user_selected_option = INVALID_OPTION;
    }

    // Free the memory
    free(buffer);

    return user_selected_option;
}

// TODO consider request parameters or the requested endpoint
void get_user_option_and_forward_to_desired_service(long *client_handler_ptr)
{
    char *buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));

    bool valid_user_input = true;
    int user_option;
    do
    {
        user_option = get_user_selected_option(client_handler_ptr);

        buffer[0] = user_option;

        // Send the user's option to the client
        if (send(*client_handler_ptr, buffer, 1, 0) < 0)
        {
            perror("Failed to send the user's option");
            exit(EXIT_FAILURE);
        }

        switch (user_option)
        {
        case LOGIN:
            login_client(client_handler_ptr, buffer);
            valid_user_input = true;
            break;
        case REGISTER:
            register_client(client_handler_ptr, buffer);
            valid_user_input = true;
            break;
        case LOGOUT:
            logout_client(client_handler_ptr, buffer);
            valid_user_input = true;
            break;
        case GUEST:
            enter_cli_as_guest(client_handler_ptr, buffer);
            valid_user_input = true;
            break;
        case EXIT:
            strcpy(buffer, "Exiting... have a nice day :)");
            if (send(*client_handler_ptr, buffer, strlen(buffer), 0) < 0)
            {
                perror("Failed to send the interface");
                exit(EXIT_FAILURE);
            }
            valid_user_input = true;
            return;
        case INVALID_OPTION:
            strcpy(buffer, "Invalid option. Please, try again.\n\n");
            if (send(*client_handler_ptr, buffer, strlen(buffer), 0) < 0)
            {
                perror("Failed to send the interface");
                exit(EXIT_FAILURE);
            }
            valid_user_input = false;
            break;
        default:
            // TODO report this cases (store in the DB, maybe)
            perror("Unexpected error during the options analysis occurred.");
            break;
        }
    } while (valid_user_input == false);

    // Free the memory
    free(buffer);
}

void introduce_options_to_the_client(long *client_handler_ptr)
{
    char *buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    sprintf(buffer,
            "Welcome to %s!\n"
            "(%d) Login\n"
            "(%d) Register\n"
            "(%d) Logout\n"
            "(%d) Guest\n"
            "(%d) Exit\n",
            (char *)SERVER_NAME, LOGIN, REGISTER, LOGOUT, GUEST, EXIT);

    // Send to the client
    if (send(*client_handler_ptr, buffer, strlen(buffer), 0) < 0)
    {
        perror("Failed to send the interface");
        exit(EXIT_FAILURE);
    }

    // Free the memory
    free(buffer);
}

void handle_client_requests(void *client_handler_ptr_arg)
{
    long *client_handler_ptr = (long *)client_handler_ptr_arg;
    char *buffer = (char *)malloc(BUFFER_SIZE);

    introduce_options_to_the_client(client_handler_ptr);
    get_user_option_and_forward_to_desired_service(client_handler_ptr);
}
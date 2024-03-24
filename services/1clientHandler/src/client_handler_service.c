#include "client_handler_service.h"

void exchange_data_with_other_service_socket(long *client_handler_ptr, char* buffer, uniSocket *serviceSocketStruct) {
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);

    size_t bytes_read;
    while ((bytes_read = recv(serviceSocketStruct->sock_fd, buffer, DEFAULT_BUFFER_SIZE, 0)) > 0) {

        // Send and receive to the client
        send(*client_handler_ptr, buffer, strlen(buffer), 0);
        memset(buffer, 0, DEFAULT_BUFFER_SIZE);
        recv(*client_handler_ptr, buffer, DEFAULT_BUFFER_SIZE, 0);

        // Send to the service
        send(serviceSocketStruct->sock_fd, buffer, strlen(buffer), 0);
    }

    // Warn about what happened due to the connection
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
    //
    const char *message;
    if (bytes_read < 0) {
        message = "Service exited due to an error";
    }
    else {
        message = "Service finished";
    }
    //
    strcpy(buffer, message);
    send(*client_handler_ptr, buffer, strlen(message), 0);

    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
}

void proxy_client_to_desired_service(long *client_handler_ptr, char* buffer, int service_option) {
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);

    switch (service_option)
    {
    case BROADCAST_SERVICE:
        uniSocket *serviceSocketStruct = create_socket(false, BROADCAST_CHAT_PORT, true);
        exchange_data_with_other_service_socket(client_handler_ptr, buffer, serviceSocketStruct);
        close_server_socket(serviceSocketStruct);
        break;
    default:
        break;
    }

    // Redirect to the options again
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
    get_user_selected_option(client_handler_ptr, buffer);
}

int get_user_selected_option(long *client_handler_ptr, char *buffer)
{
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
    int user_selected_option;

    // Send the prompting line
    strcpy(buffer, "Enter the number of the desired option: ");
    send(*client_handler_ptr, buffer, strlen(buffer), 0);

    // Get input
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
    size_t bytes_received;
    if ((bytes_received = recv(*client_handler_ptr, buffer, DEFAULT_BUFFER_SIZE, 0)) < 0)
    {
        fprintf(stderr, "recv() method exited with error %d\n", (int)bytes_received);
        exit(EXIT_FAILURE);
    }

    // Sanitize the input
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

    memset(buffer, 0, DEFAULT_BUFFER_SIZE);

    return user_selected_option;
}

void get_user_option_and_forward_to_desired_service(long *client_handler_ptr, char* buffer)
{
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);

    bool valid_user_input = true;
    int user_option;
    do
    {
        user_option = get_user_selected_option(client_handler_ptr, buffer);

        // Show the client the considered option
        sprintf(buffer,"Option %d selected.\n", user_option);
        if (send(*client_handler_ptr, buffer, 1, 0) < 0)
        {
            perror("Failed to send the user's option");
            exit(EXIT_FAILURE);
        }

        // Redirect to the desired service
        switch (user_option)
        {
        case LOGIN:
            // login_client(client_handler_ptr, buffer);
            valid_user_input = true;
            break;
        case REGISTER:
            // register_client(client_handler_ptr, buffer);
            valid_user_input = true;
            break;
        case LOGOUT:
            // logout_client(client_handler_ptr, buffer);
            valid_user_input = true;
            break;
        case GUEST:
            proxy_client_to_desired_service(client_handler_ptr, buffer, BROADCAST_SERVICE);
            // enter_cli_as_guest(client_handler_ptr, buffer);
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

    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
}

void introduce_options_to_the_client(long *client_handler_ptr, char* buffer)
{
    memset(buffer, 0, DEFAULT_BUFFER_SIZE);

    sprintf(buffer,
            "Welcome to %s!\n"
            "(%d) Login\n"
            "(%d) Register\n"
            "(%d) Logout\n"
            "(%d) Guest\n"
            "(%d) Exit\n",
            (char *)SERVER_NAME, LOGIN, REGISTER, LOGOUT, GUEST, EXIT);

    if (send(*client_handler_ptr, buffer, strlen(buffer), 0) < 0)
    {
        perror("Failed to send the interface\n");
        exit(EXIT_FAILURE);
    }

    memset(buffer, 0, DEFAULT_BUFFER_SIZE);
}

void handle_client_requests(void *client_handler_ptr_arg)
{
    char *buffer = (char *)calloc(DEFAULT_BUFFER_SIZE, sizeof(char));

    long *client_handler_ptr = (long *)client_handler_ptr_arg;
    introduce_options_to_the_client(client_handler_ptr, buffer);
    get_user_option_and_forward_to_desired_service(client_handler_ptr, buffer);

    free(buffer);
}
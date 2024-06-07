#include "ftp_service.h"

// Let only a thread at a time to access the main service struct
static pthread_mutex_t g_mutex_server = PTHREAD_MUTEX_INITIALIZER;

int list_files_curr_dir(ClientInfo_t *p_client_t)
{
    DIR *directory;
    struct dirent *entry;
    directory = opendir(PATH_ASSETS_FOLDER);
    if (directory)
    {
        int file_idx = 0;
        while ((entry = readdir(directory)) != NULL)
        {
            memset(p_client_t->buffer, 0, BUFFER_SIZE);
            sprintf(p_client_t->buffer, "(%d) %s\n", file_idx, entry->d_name);
            if (send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0) < 0) {
                ERROR_VERBOSE_LOG("Error sending filename to the client");
                return -1;
            }
            file_idx++;
        }
        closedir(directory);
        return 0;
    }
    else
    {
        ERROR_VERBOSE_LOG("Error opening the files directory");
        return 1;
    }

    return 0;
}

int download_file(ClientInfo_t *p_client_t)
{ 
    memset(p_client_t->buffer, 0, BUFFER_SIZE);

    // Receive the FILENAME
    fill_buffer_with_response(p_client_t);
    char *filename = strdup(p_client_t->buffer);
    filename[strlen(filename) - 1] = '\0';

    // Send the FILESIZE to the client
    char *file_complete_path = (char *)malloc(MAX_LEN_FILE_PATH);
    strcpy(file_complete_path, PATH_ASSETS_FOLDER);
    strcat(file_complete_path, filename);
    //
    off_t filesize = get_file_size(file_complete_path);
    if (filesize < 0) {
        printf("Error calculating the file's size.\n");
        printf("Please, try again.\n");
        //TODO send this message to the client
        return 0;
    }
    //
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    char filesize_str[MAX_NUM_ALGS_FILESIZE]; 
    snprintf(p_client_t->buffer, sizeof(filesize_str), "%ld", filesize); // more robust than (char) typecaste
    if (send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0) < 0) { //TODO: considerar bem esta PARTEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
        ERROR_VERBOSE_LOG("Error sending content to the client");
        return -1;
    }
    
    // Send the file to the client
    FILE *p_file = fopen(file_complete_path, "rb"); 
    if (p_file == NULL) {
        ERROR_VERBOSE_LOG("Invalid file pointer\n");
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
        sprintf(p_client_t->buffer, "File %s not found. Please, try again.\n", filename);
        if (send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0) < 0) {
            ERROR_VERBOSE_LOG("Error sending the filesize to the client");
            return -2;
        }
        free(filename);
        return -1;
    }
    //
    INFO_VERBOSE_LOG("Sending the file.");
    off_t remaining_to_send = filesize;
    while (remaining_to_send > 0)
    {
        size_t amount_to_send = remaining_to_send > BUFFER_SIZE ? BUFFER_SIZE : remaining_to_send;
        size_t bytes_read = fread(p_client_t->buffer, 1, amount_to_send, p_file);
        if (bytes_read <= 0) {
            ERROR_VERBOSE_LOG("Error reading content from the file to upload\n");
            fclose(p_file);
            free(filename);
            return -1;
        }
        send(p_client_t->sock_FD, p_client_t->buffer, bytes_read, 0);
        remaining_to_send -= bytes_read;
    }
    fclose(p_file);
    INFO_VERBOSE_LOG("File sent.");

    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    return 0;
}

int upload_file(ClientInfo_t *p_client_t) {
    memset(p_client_t->buffer, 0, BUFFER_SIZE);

    // Receive the FILENAME
    if (fill_buffer_with_response(p_client_t) < 0) {
        ERROR_VERBOSE_LOG("Error receiving the FILENAME from the client\n");
        return -1;
    }
    char *filename = strdup(p_client_t->buffer);
    if (filename == NULL) {
        ERROR_VERBOSE_LOG("Memory allocation error\n");
        return -1;
    }

    // Remove newline character from filename
    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    // Receive the FILESIZE
    if (fill_buffer_with_response(p_client_t) < 0) {
        ERROR_VERBOSE_LOG("Error receiving the FILESIZE from the client\n");
        free(filename);
        return -2;
    }
    int filesize = atoi(p_client_t->buffer);

    if (filesize > MAX_FILE_SIZE) {
        ERROR_VERBOSE_LOG("The file size exceeds the limit. Please, consider using a smaller file.\n");
        free(filename);
        return -3;
    }

    char file_complete_path[MAX_LEN_FILE_PATH];
    snprintf(file_complete_path, sizeof(file_complete_path), "%s%s", PATH_ASSETS_FOLDER, filename);

    FILE *p_file = fopen(file_complete_path, "wb");
    if (p_file == NULL) {
        ERROR_VERBOSE_LOG("Error creating the file from the server side\n");
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
        snprintf(p_client_t->buffer, BUFFER_SIZE, "File '%s' may already exist.\n", filename);
        if (send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0) < 0) {
            ERROR_VERBOSE_LOG("Error sending the filesize to the client");
            return -4;
        }
        free(filename);
        return -5;
    }

    int remaining_to_recv = filesize;
    while (remaining_to_recv > 0) {
        size_t amount_to_recv = remaining_to_recv > BUFFER_SIZE ? BUFFER_SIZE : remaining_to_recv;
        ssize_t bytes_received = recv(p_client_t->sock_FD, p_client_t->buffer, amount_to_recv, 0);
        if (bytes_received < 0) {
            ERROR_VERBOSE_LOG("Error receiving content from the client\n");
            fclose(p_file);
            free(filename);
            return -1;
        }
        fwrite(p_client_t->buffer, 1, bytes_received, p_file);
        remaining_to_recv -= bytes_received;
    }

    fclose(p_file);

    // Notify the user about the state
    const char *final_msg = "File uploaded!\n";
    send(p_client_t->sock_FD, final_msg, strlen(final_msg), 0);

    free(filename);
    return 0;
}

int inform_client(ClientInfo_t *p_client_t)
{
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    sprintf(p_client_t->buffer,
            "\t%s (%s)\n"
            "\t%s (%s)\n"
            "\t%s (%s)\n"
            "\t%s (finish the session)\n",
            CMD_UPLOAD_SHORT, CMD_UPLOAD_FULL,
            CMD_DOWNLOAD_SHORT, CMD_DOWNLOAD_FULL,
            CMD_LIST_SHORT, CMD_LIST_FULL,
            CMD_EXIT_FULL);
    if (send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0) < 0) {
        ERROR_VERBOSE_LOG("Error sending help options to the client");
        return -1;
    }

    return 0;
}

int input_client_commands(ClientInfo_t *p_client_t)
{
    if (p_client_t == NULL)
    {
        perror("Invalid pointer to client struct");
        return -1;
    }

    // Inform the client with the main information
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    sprintf(p_client_t->buffer,
            "Welcome to the uploading service, %s!\n\n"
            "Insert the desired command after the '>'.\n"
            "Enter '%s' (or '%s') for available commands.\n",
            p_client_t->name, CMD_HELP_SHORT, CMD_HELP_FULL);
    
    if (send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0) < 0) {
        ERROR_VERBOSE_LOG("Error sending the available options to the client");
        return -2;
    }

    // Interpret the option and forward
    bool clients_wants_exit = false;
    do
    {
        memset(p_client_t->buffer, 0, BUFFER_SIZE); // TODO fazer método genérico para isto
        const char *file_sent_msg = "> ";
        strncpy(p_client_t->buffer, file_sent_msg, strlen(file_sent_msg));
        if (send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0) < 0) {
            ERROR_VERBOSE_LOG("Error sending the available options to the client");
            return -3;
        }

        if (fill_buffer_with_response(p_client_t) == CLIENT_DISCONNECTED)
        {
            return 0;
        }
        //
        char *feedback_msg = NULL; // TODO define
        //
        //
        if (strcmp(p_client_t->buffer, CMD_HELP_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_HELP_FULL) == 0)
        {
            inform_client(p_client_t);
        }
        else if (strcmp(p_client_t->buffer, CMD_UPLOAD_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_UPLOAD_FULL) == 0)
        {
            upload_file(p_client_t);
        }
        else if (strcmp(p_client_t->buffer, CMD_DOWNLOAD_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_DOWNLOAD_FULL) == 0)
        {
            download_file(p_client_t);
        }
        else if (strcmp(p_client_t->buffer, CMD_EXIT_FULL) == 0)
        {
            feedback_msg = "'--exit' option selected. Have a nice day!\n";
            clients_wants_exit = true; // end the cycle, and finish the service
        }
        else if (strcmp(p_client_t->buffer, CMD_LIST_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_LIST_FULL) == 0)
        {
            list_files_curr_dir(p_client_t);
        }
        else
        {
            feedback_msg = "Invalid option. Please, try again.\n";
        }

        // Give feedback to the client
        if (feedback_msg != NULL)
        {
            memset(p_client_t->buffer, 0, BUFFER_SIZE);
            strncpy(p_client_t->buffer, feedback_msg, strlen(feedback_msg));
            if (send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0) < 0) {
                ERROR_VERBOSE_LOG("Error sending the feedback msg of the option to the client");
                return -2;
            }
        }

    } while (clients_wants_exit == false);

    return 0;
}


int ask_client_basic_details(ClientInfo_t *p_client_t)
{
    const char *init_msg = "This is the FTP server. How can we call you? ";
    send(p_client_t->sock_FD, init_msg, strlen(init_msg), 0);

    // Receive the pretended username
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    int bytes_received;
    if ((bytes_received = recv(p_client_t->sock_FD, p_client_t->buffer, BUFFER_SIZE, 0)) < 0)
    {
        ERROR_VERBOSE_LOG("Error receiving the preferred name from the client");
        return -1;
    }
    else if (bytes_received == 0)
    {
        printf("Client terminated the connection.\n");
        return 0;
    }

    // Sanitize the input (and allow for a correct comparison of names)
    if (bytes_received <= BUFFER_SIZE)
    {
        p_client_t->buffer[bytes_received - 1] = '\0';
    }

    // Store the username in the client struct
    strncpy(p_client_t->name, p_client_t->buffer, strlen(p_client_t->buffer));

    return 0;
}


void *prepare_client_structs_for_data(ClientInfo_t *p_client_t)
{
    if (p_client_t == NULL)
    {
        ERROR_VERBOSE_LOG("Received a pointer pointing to improperly allocated memory");
        return NULL;
    }
    //
    p_client_t->name = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (p_client_t->name == NULL)
    {
        ERROR_VERBOSE_LOG("Error alocating memory for the p_client_t struct name");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return NULL;
    }

    p_client_t->buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (p_client_t->buffer == NULL)
    {
        ERROR_VERBOSE_LOG("Error alocating memory for the buffer pointer");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return NULL;
    }

    // Get the client's IP address (for a unique identification)
    char ip_buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, (void *)&(p_client_t->p_addr), ip_buffer, INET_ADDRSTRLEN) == NULL) // TODO handle IPV6 too
    {
        ERROR_VERBOSE_LOG("Error converting IP address");
        free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
        return NULL;
    }
    //
    p_client_t->addr_info = ip_buffer;
}

// UniSocket_t *get_transference_socket()
// {

//     // TODO usar o novo standard que tenho do lado do cliente
//     UniSocket_t *p_server_content_t;
//     if (((p_server_content_t = create_socket_struct(true, DATA_PORT, false)) == NULL)) // last false = DUAL stack socket
//     {
//         ERROR_VERBOSE_LOG("Error getting the socket struct pointer");
//         return NULL;
//     }

//     return p_server_content_t;
// }

int serve_client_with_FTP(ClientInfo_t *p_client_t)
{
    // Allows the server to continue, 
    // even when client disconnects abruptely
    signal(SIGPIPE, SIG_IGN); 

    if (prepare_client_structs_for_data(p_client_t) == NULL)
    {
        ERROR_VERBOSE_LOG("Error preparing client struct for data");
        const char *error_msg = "There was an error preparing your connection. Sorry. Please, try again.\n";
        send(p_client_t->sock_FD, error_msg, strlen(error_msg), 0);
        exit(EXIT_FAILURE);
    }

    int asking_status;
    if (asking_status = ask_client_basic_details(p_client_t) < 0)
    {
        ERROR_VERBOSE_LOG("Error asking the client's name");
        exit(EXIT_FAILURE);
        return asking_status;
    }

    int input_command_status;
    if (input_command_status = input_client_commands(p_client_t) < 0)
    {
        ERROR_VERBOSE_LOG("Error while receiving client's commands");
        return input_command_status;
    }

    free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
}
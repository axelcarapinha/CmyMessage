#include "ftp_service.h"

// Got some inspiration at https://www.rfc-editor.org/rfc/rfc765

// 8 bits of byte size
// passive data port, and active one
// EOF
// EOR
// restart file transfer when error
// FTP commandsz
// NVT
// NVFS

// Let only a thread at a time to access the main service struct
static pthread_mutex_t g_mutex_server = PTHREAD_MUTEX_INITIALIZER;

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
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
            send_text_to_client_with_buffer(p_client_t);
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

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */




//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */

int download_file(ClientInfo_t *p_client_t)
{ 
    memset(p_client_t->buffer, 0, BUFFER_SIZE);

    // Receive the FILENAME
    fill_cli_buffer_with_response(p_client_t);
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
    snprintf(p_client_t->buffer, sizeof(filesize_str), "%ld", filesize); // more robust than (char) typecaster
    send_text_to_client_with_buffer(p_client_t);
    
    // Send the file to the client
    FILE *p_file = fopen(file_complete_path, "rb"); 
    if (p_file == NULL) {
        ERROR_VERBOSE_LOG("Invalid file pointer\n");
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
        sprintf(p_client_t->buffer, "File %s not found. Please, try again.\n", filename);
        send_text_to_client_with_buffer(p_client_t);
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

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Based on the order of implementation (from the client side too)
 * @param
 *
 * @return
 */
int upload_file(ClientInfo_t *p_client_t) {
    memset(p_client_t->buffer, 0, BUFFER_SIZE);

    // Receive the FILENAME
    if (fill_cli_buffer_with_response(p_client_t) < 0) {
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
    if (fill_cli_buffer_with_response(p_client_t) < 0) {
        ERROR_VERBOSE_LOG("Error receiving the FILESIZE from the client\n");
        free(filename);
        return -2;
    }
    int filesize = atoi(p_client_t->buffer);

    if (filesize > MAX_FILE_SIZE) {
        ERROR_VERBOSE_LOG("The file size exceeds the limit. Please, consider using a smaller file.\n");
        free(filename);
        return -2;
    }

    char file_complete_path[MAX_LEN_FILE_PATH];
    snprintf(file_complete_path, sizeof(file_complete_path), "%s%s", PATH_ASSETS_FOLDER, filename);

    FILE *p_file = fopen(file_complete_path, "wb");
    if (p_file == NULL) {
        ERROR_VERBOSE_LOG("Error creating the file from the server side\n");
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
        snprintf(p_client_t->buffer, BUFFER_SIZE, "File '%s' may already exist.\n", filename);
        send_text_to_client_with_buffer(p_client_t);
        free(filename);
        return -1;
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


//----------------------------------------------------------------------------------------------------------
// TODO do figlet
/**
 * @brief

 * @param
 *
 * @return
 */
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
    int send_status;
    if ((send_status = send_text_to_client_with_buffer(p_client_t)) < 0)
    {
        ERROR_VERBOSE_LOG("Error sending help options to the client");
        return send_status;
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int compress_file()
{

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int parse_client_command_and_redirect(int command)
{

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int define_acess_controls(ClientInfo_t *p_client_t)
{

    return 0;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
off_t get_file_size(const char *file_complete_path) // Support all systems (NOT only POSIX respecting ones)
{
    struct stat file_stat; // file's status
    off_t size;

    if (access(file_complete_path, F_OK) != 0) {
        printf("File %s does NOT exist.\n", file_complete_path);
        return -1;
    }
    else if (strlen(file_complete_path) > (MAX_LEN_FILE_PATH - strlen(PATH_ASSETS_FOLDER) - 1)) {
        printf("File path is too long\n");
        return -2;
    }
    else if (stat(file_complete_path, &file_stat) == -1)
    {
        printf("Invalid path for the file\n");
        return -3; 
    }

    FILE *p_file = fopen(file_complete_path, "rb"); //TODO name the file pointers this way everywhere
    if (p_file == NULL) {
        ERROR_VERBOSE_LOG("Error opening the file");
        return -1;
    }
    fseek(p_file, 0, SEEK_END); 
    size = ftell(p_file); 
    fclose(p_file); //TODO make sure the file pointer is closed on more code (valgrind, ...)
    
    return size;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param p_client_t
 * @return int
 */
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
    send_text_to_client_with_buffer(p_client_t);

    // Interpret the option and forward
    bool clients_wants_exit = false;
    do
    {
        memset(p_client_t->buffer, 0, BUFFER_SIZE); // TODO fazer método genérico para isto
        const char *file_sent_msg = "> ";
        strncpy(p_client_t->buffer, file_sent_msg, strlen(file_sent_msg));
        send_text_to_client_with_buffer(p_client_t);
        if (fill_cli_buffer_with_response(p_client_t) == CLIENT_DISCONNECTED)
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
            clients_wants_exit = true; // end the cycle, and finisht the service
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
            send_text_to_client_with_buffer(p_client_t);
        }

    } while (clients_wants_exit == false);

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */

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

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
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

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
UniSocket_t *get_transference_socket()
{

    // TODO usar o novo standard que tenho do lado do cliente
    UniSocket_t *p_server_content_t;
    if (((p_server_content_t = create_socket_struct(true, CONTENT_PORT, false)) == NULL)) // last false = DUAL stack socket
    {
        ERROR_VERBOSE_LOG("Error getting the socket struct pointer");
        return NULL;
    }

    return p_server_content_t;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */

int serve_client_with_FTP(ClientInfo_t *p_client_t)
{
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
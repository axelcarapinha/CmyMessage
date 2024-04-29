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
// 

// Let only a thread at a time to access the main service struct
static pthread_mutex_t g_mutex_server = PTHREAD_MUTEX_INITIALIZER;


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int download_file(ClientInfo_t *p_client_t) {

    // ASK for the filename
    memset(p_client_t->buffer, 0, BUFFER_SIZE); //TODO retirar depois de funcionar
    const char *filename_msg = "Filename: ";
    strncpy(p_client_t->buffer, filename_msg, strlen(filename_msg));
    send_text_to_client_with_buffer(p_client_t);

    // GET filename
    if (fill_cli_buffer_with_response(p_client_t) < 0) {
        ERROR_VERBOSE_LOG("Error receiving the pretended filename");
        return -1;
    }
    // (the input was already parsed by the wrapper function)
    char *filename = (char *)malloc(sizeof(char) * FILENAME_MAX); //TODO check the size of this
    strncpy(filename, p_client_t->buffer, strlen(p_client_t->buffer));

    // Send the file to the client
    FILE *file_ptr;
    file_ptr = fopen(filename, "rb");
    if (file_ptr == NULL) {
        ERROR_VERBOSE_LOG("Error creating the filename");
        //
        memset(p_client_t->buffer, 0, BUFFER_SIZE); 
        const char *not_found_msg = "File not found. Please, try again.\n";
        strncpy(p_client_t->buffer, not_found_msg, strlen(not_found_msg));
        send_text_to_client_with_buffer(p_client_t);   
        return 0;
    }
    //
    memset(p_client_t->buffer, 0, BUFFER_SIZE); //TODO fazer método genérico para isto
    const char *message = "Sending file...\n";
    strncpy(p_client_t->buffer, message, strlen(message));
    send_text_to_client_with_buffer(p_client_t);   
    while (fgets(p_client_t->buffer, sizeof(p_client_t->buffer), file_ptr) != NULL) {
        send_text_to_client_with_buffer(p_client_t);   
        //TODO avoid path traversal vulnerabilities
    }
    memset(p_client_t->buffer, 0, BUFFER_SIZE); //TODO fazer método genérico para isto
    const char *file_sent_msg = "File sent...\n";
    strncpy(p_client_t->buffer, file_sent_msg, strlen(file_sent_msg));
    send_text_to_client_with_buffer(p_client_t); 

    return 0;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int upload_file(ClientInfo_t *p_client_t) {
  

    return 0;
}


//----------------------------------------------------------------------------------------------------------
//TODO do figlet
/**
 * @brief

 * @param
 *
 * @return
 */
int inform_client(ClientInfo_t *p_client_t) {
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    sprintf(p_client_t->buffer, 
        "\t%s (%s)\n"
        "\t%s (%s)\n"
        "\t%s (finish the session)\n",
        CMD_UPLOAD_SHORT, CMD_UPLOAD_FULL,
        CMD_DOWNLOAD_SHORT, CMD_DOWNLOAD_FULL,
        CMD_EXIT_FULL
        );
    int send_status;
    if ((send_status = send_text_to_client_with_buffer(p_client_t)) < 0) {
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
int compress_file() {
    

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int parse_client_command_and_redirect(int command) {
   


    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int define_acess_controls(ClientInfo_t *p_client_t) {
    
    return 0;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief 
 * 
 * @param p_client_t 
 * @return int 
 */
int input_client_commands(ClientInfo_t *p_client_t) {
    if (p_client_t == NULL) {
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
    do {
        memset(p_client_t->buffer, 0, BUFFER_SIZE); //TODO fazer método genérico para isto
        const char *file_sent_msg = "> ";
        strncpy(p_client_t->buffer, file_sent_msg, strlen(file_sent_msg));
        send_text_to_client_with_buffer(p_client_t); 
        if (fill_cli_buffer_with_response(p_client_t) == CLIENT_DISCONNECTED) {
            return 0;
        }
        //
        char *feedback_msg = NULL; //TODO define
        //
        //
        if (strcmp(p_client_t->buffer, CMD_HELP_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_HELP_FULL) == 0) {
            inform_client(p_client_t);
        }
        else if (strcmp(p_client_t->buffer, CMD_UPLOAD_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_UPLOAD_FULL) == 0) {
            upload_file(p_client_t);
            feedback_msg = "File uploaded.\n";
        }
        else if (strcmp(p_client_t->buffer, CMD_DOWNLOAD_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_DOWNLOAD_FULL) == 0) {
            download_file(p_client_t);
        }
        else if (strcmp(p_client_t->buffer, CMD_EXIT_FULL) == 0) {
            feedback_msg = "'--exit' option selected. Have a nice day!\n";
            clients_wants_exit = true; // end the cycle, and finisht the server
        }
        else {
            feedback_msg = "Invalid option. Please, try again.\n";
        }

        // Give feedback to the client
        if (feedback_msg != NULL) {
            memset(p_client_t->buffer, 0, BUFFER_SIZE);
            strncpy(p_client_t->buffer, feedback_msg, strlen(feedback_msg));
            send_text_to_client_with_buffer(p_client_t);
        }

    } while(clients_wants_exit == false);
    
    return 0;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */

int ask_client_basic_details(ClientInfo_t *p_client_t) {
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
    if (inet_ntop(AF_INET, (void *)&(p_client_t->p_addr), ip_buffer, INET_ADDRSTRLEN) == NULL) //TODO handle IPV6 too
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

int serve_client_with_FTP(ClientInfo_t *p_client_t)
{
    if (prepare_client_structs_for_data(p_client_t) == NULL) {
        ERROR_VERBOSE_LOG("Error preparing client struct for data");
        const char *error_msg = "There was an error preparing your connection. Sorry. Please, try again.\n";
        send(p_client_t->sock_FD, error_msg, strlen(error_msg), 0);
        exit(EXIT_FAILURE);
    }

    int asking_status;
    if (asking_status = ask_client_basic_details(p_client_t) < 0) {
        ERROR_VERBOSE_LOG("Error asking the client's name");
        exit(EXIT_FAILURE);
        return asking_status;
    }

    int input_command_status;
    if (input_command_status = input_client_commands(p_client_t) < 0) {
        ERROR_VERBOSE_LOG("Error while receiving client's commands");
        return input_command_status;
    }

    free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
}
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
int open_file() {


    return 0;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int send_file() {
    

    return 0;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int write_file() {


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

 * @param
 *
 * @return
 */
int send_customized_welcome_message(ClientInfo_t *p_client_t)
{
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    sprintf(p_client_t->buffer, "Welcome to the uploading service, %s!\n", p_client_t->name);

    int sending_status;
    if ((sending_status = send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0)) < 0)
    {
        ERROR_VERBOSE_LOG("Error sending the customized welcome message");
        return sending_status;
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



//TODO:
//  // Send the corresponding code to the server
//     int option = INVALID_OPTION;
//     //
//     if (strcmp(p_client_t->buffer, CMD_HELP_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_HELP_FULL) == 0) {
//         option = HELP;
//     }
//     //
//     else if (strcmp(p_client_t->buffer, CMD_UPLOAD_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_UPLOAD_FULL) == 0) {
//         option = UPLOAD;
//     }
//     else if (strcmp(p_client_t->buffer, CMD_DOWNLOAD_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_DOWNLOAD_FULL) == 0) {
//         option = DOWNLOAD;
//     }


    // int input_commands_status;
    // if (input_commands_status = input_client_commands(p_client_t)) {
    //     perror("Error receiving the pretended commands from the client");
    //     return input_commands_status;
    // }

// int input_client_commands(ClientInfo_t *p_client_t) {

//     // Input the desired command
//     printf("Insert the desired command after the '>'\n");
//     printf("Enter '%s' (or '%s') to know the available commands\n", CMD_HELP_SHORT, CMD_HELP_FULL);
//     fgets(p_client_t->buffer, BUFFER_SIZE, stdin);

//     // Send the option to the FTP server
//     p_client_t->buffer[BUFFER_SIZE] = '\0';
//     send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0);

//     //     

    
//     return 0;
// }



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
    
    int welcoming_status;
    if (welcoming_status = send_customized_welcome_message(p_client_t) < 0) {
        ERROR_VERBOSE_LOG("Error sending a custom welcome message to the client");
        exit(EXIT_FAILURE);
    }

    //TODO
    // int input_command_status;
    // if (input_command_status = input_client_command(p_client_t) < 0) {
    //     ERROR_VERBOSE_LOG("Error while handling client comments");
    //     return input_command_status;
    // }

    free_client_memory_with_ptr_to_ptr((void **)&p_client_t);
}
#include "ftp_client.h"

// regex_t exit_regex;
//     if (regcomp(&exit_regex, "([eE][xX][iI][tT])", REG_EXTENDED) != 0)
//     {
//         fprintf(stderr, "Failed to compile regex\n");
//         exit(EXIT_FAILURE);
//     }



//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int keep_connection_with_server_cli(ClientInfo_t *p_client_t) { //!TODO make this a generalized function (more used)

    //  Prepare the structure to maintain the client-server interaction
    fd_set comm_sets; 
    FD_ZERO(&comm_sets);
    FD_SET(0, &comm_sets); // stdin (client input)
    FD_SET(p_client_t->sock_FD, &comm_sets);

    while (true) { 
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
        fd_set curr_set = comm_sets;
        if (select(p_client_t->sock_FD + 1, &curr_set, NULL, NULL, NULL) < 0) { // +1 to include the check for the client socket
            ERROR_VERBOSE_LOG("Error when selecting the descriptors");
            return -1;
        }

        if (FD_ISSET(0, &curr_set)) { 
            // Respond to the server
            if (fgets(p_client_t->buffer, BUFFER_SIZE, stdin) == NULL) {
                ERROR_VERBOSE_LOG("Error reading input from the client (stdin)");
                return -2;
            }
            send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0);
        }
        else if (FD_ISSET(p_client_t->sock_FD, &curr_set)) { 
            // Get the server's content
            int num_bytes = recv(p_client_t->sock_FD, p_client_t->buffer, BUFFER_SIZE, 0);
            if (num_bytes < 0) {
                ERROR_VERBOSE_LOG("Error receiving the contents from the server\n");
                return -3;
            }
            else if (num_bytes == 0) {
                printf("Connection terminated by the server\n");
                return 0;
            }
            p_client_t->buffer[num_bytes] = '\0';
            printf("%s", p_client_t->buffer); 
            fflush(stdout);
        }
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
int upload_file(ClientInfo_t *p_client_t) { //TODO consider generalzing the function 

    // // ASK for the filename
    // memset(p_client_t->buffer, 0, BUFFER_SIZE); //TODO retirar depois de funcionar
    // const char *filename_msg = "Filename: ";
    // strncpy(p_client_t->buffer, filename_msg, strlen(filename_msg));
    // send_text_to_client_with_buffer(p_client_t);

    // // GET filename
    // if (fill_cli_buffer_with_response(p_client_t) < 0) {
    //     ERROR_VERBOSE_LOG("Error receiving the pretended filename");
    //     return -1;
    // }
    // // (the input was already parsed by the wrapper function)
    // char *filename = (char *)malloc(sizeof(char) * FILENAME_MAX); //TODO check the size of this
    // p_client_t->buffer[strlen(p_client_t->buffer)] = '\0';
    // strncpy(filename, p_client_t->buffer, strlen(p_client_t->buffer));

    // // Send the file to the client
    // memset(p_client_t->buffer, 0, BUFFER_SIZE); 
    // strcat(p_client_t->buffer, ASSETS_FOLDER_NAME);
    // strcat(p_client_t->buffer, filename);
    // FILE *file_ptr = fopen(p_client_t->buffer, "rb"); //TODO size of the file
    // if (file_ptr == NULL) {
    //     ERROR_VERBOSE_LOG("Error creating the filename");
    //     //
    //     memset(p_client_t->buffer, 0, BUFFER_SIZE); 
    //     sprintf(p_client_t->buffer, "File %s not found. Please, try again.\n", filename);
    //     send_text_to_client_with_buffer(p_client_t);   
    //     return 0;
    // }
    
    // memset(p_client_t->buffer, 0, BUFFER_SIZE); //TODO fazer método genérico para isto
    // const char *message = "Sending file...\n";
    // strncpy(p_client_t->buffer, message, strlen(message));
    // send_text_to_client_with_buffer(p_client_t);   
    // while (fgets(p_client_t->buffer, sizeof(p_client_t->buffer), file_ptr) != NULL) {
    //     send_text_to_client_with_buffer(p_client_t);   
    //     //TODO avoid path traversal vulnerabilities
    // }
    // memset(p_client_t->buffer, 0, BUFFER_SIZE); //TODO generalize this into a function
    // const char *file_sent_msg = "File sent...\n";
    // strncpy(p_client_t->buffer, file_sent_msg, strlen(file_sent_msg));
    // send_text_to_client_with_buffer(p_client_t); 

    // return 0;
}



//----------------------------------------------------------------------------------------------------------
/**
 * @brief 
 * 
 * @return int 
 */
// int get_file_size() {
//     struct stat statistic;
//     stat(filename, &st);

//     FILE *file_ptr = fopen(p_client_t->buffer, "wb"); 
//     if (file_ptr == NULL) {
//         ERROR_VERBOSE_LOG("Error creating the file from the server side.");
//         //
//         memset(p_client_t->buffer, 0, BUFFER_SIZE); 
//         sprintf(p_client_t->buffer, "File '%s' may already exist.\n", filename);
//         send_text_to_client_with_buffer(p_client_t);
//         return 0;
//     }



//     size = st.st_size;
//     . . .
//     // OR (caso a plataforma não seja POSIX
//     fseek(f, 0, SEEK_END); // seek to end of file
//     size = ftell(f); // get current file pointer
//     fseek(f, 0, SEEK_SET); // seek back to beginning of file
// }




//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int start_using_FTP(ClientInfo_t *p_client_t) {
    if (p_client_t == NULL) {
        perror("Invalid pointer to the client struct");
        return -1;
    }

    int connection_status;
    if ((connection_status = keep_connection_with_server_cli(p_client_t)) < 0) {
        perror("Error at some point during the connection");
        return connection_status;
    }

    return 0;
}
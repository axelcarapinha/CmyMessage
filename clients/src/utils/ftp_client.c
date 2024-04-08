#include "clients.h"

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
int input_client_commands(ClientInfo_t *p_client_t) {
    printf("Insert the desired command after the '>'\n");
    printf("Enter 'help' to know the available commands\n");
    fgets(p_client_t->buffer, BUFFER_SIZE, stdin);

    // if () {

    // }


    // connect_to_server(p_socket_t, server_port, server_ip);
    
    return 0;
}




//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int start_using_FTP(ClientInfo_t *p_client_t) {

    int input_commands_status;
    if (input_commands_status = input_client_commands(p_client_t)) {
        perror("Error receiving the pretended commands from the client");
        return input_commands_status;
    }

    //


    return 0;
}
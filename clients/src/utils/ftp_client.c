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
int keep_connection_with_server_cli(ClientInfo_t *p_client_t) {

    while (true) { //TODO error messages when recv and amount received
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
        recv(p_client_t->sock_FD, p_client_t->buffer, BUFFER_SIZE, 0);
        printf("> %s", p_client_t->buffer);
        //
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
        fgets(p_client_t->buffer, BUFFER_SIZE, stdin);
        send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer),0);
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
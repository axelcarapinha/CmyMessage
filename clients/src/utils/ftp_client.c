#include "ftp_client.h"

int download_file(ClientInfo_t *p_client_t) 
{
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    send(p_client_t->sock_FD, CMD_DOWNLOAD_FULL, strlen(CMD_DOWNLOAD_FULL), 0);

    printf("Filename: ");
    fgets(p_client_t->buffer, BUFFER_SIZE, stdin);
    const char *filename = strdup(p_client_t->buffer);
    send(p_client_t->sock_FD, filename, strlen(filename), 0);

    // Create the file at the same time (a bit more efficient, avoids one waiting for the other)
    FILE *p_file = fopen(filename, "wb");
    if (p_file == NULL)
    {
        ERROR_VERBOSE_LOG("Error creating the file from the server side");
        printf("File '%s' may already exist.\n", filename);
        return 0;
    }

    // Receive the FILESIZE
    fill_buffer_with_response(p_client_t);
    const int filesize = atoi(p_client_t->buffer);

    printf("Downloading the file...\n"); //TODO reuse this part of code!
    int remaining_to_recv = filesize;
    while (remaining_to_recv > 0) {
        size_t amount_to_recv = remaining_to_recv > BUFFER_SIZE ? BUFFER_SIZE : remaining_to_recv;
        ssize_t bytes_received = recv(p_client_t->sock_FD, p_client_t->buffer, amount_to_recv, 0);
        if (bytes_received < 0) {
            ERROR_VERBOSE_LOG("Error receiving content from the server\n");
            fclose(p_file);
            free((void *)filename);
            return -1;
        }
        fwrite(p_client_t->buffer, 1, bytes_received, p_file);
        remaining_to_recv -= bytes_received;
    }
    printf("File downloaded!\n");    
    fflush(stdout); // allow the server to display the full content of its message

    // Ensure the changes to the file get written
    fclose(p_file);

    free((void *)filename);
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    return 0;
}

int upload_file(ClientInfo_t *p_client_t)
{
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    send(p_client_t->sock_FD, CMD_UPLOAD_FULL, strlen(CMD_UPLOAD_FULL), 0);

    list_available_files(); // on this host

    printf("File to upload: ");
    if (fgets(p_client_t->buffer, BUFFER_SIZE, stdin) == NULL)
    {
        ERROR_VERBOSE_LOG("Error getting the intended filename to upload\n");
        return -1;
    }

    char *filename = strdup(p_client_t->buffer);
    if (filename == NULL) {
        ERROR_VERBOSE_LOG("Memory allocation error\n");
        return -1;
    }

    // Remove newline character
    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    char file_complete_path[MAX_LEN_FILE_PATH];
    snprintf(file_complete_path, sizeof(file_complete_path), "%s%s", PATH_ASSETS_FOLDER, filename);

    long filesize = get_file_size(file_complete_path);
    if (filesize < 0) {
        printf("Error calculating the file's size.\n");
        printf("Please, try again.\n");
        free(filename);
        return 0;
    }
    else if (filesize > MAX_FILE_SIZE)
    {
        printf("That file is enormous! This server cannot handle it (for now).\n");
        printf("Sorry. Please, try another one\n");
        free(filename);
        return 0;
    }

    // Allow the server to prepare to receive the file
    send(p_client_t->sock_FD, filename, strlen(filename), 0);

    char filesize_str[MAX_NUM_ALGS_FILESIZE]; 
    snprintf(filesize_str, sizeof(filesize_str), "%ld", filesize);
    send(p_client_t->sock_FD, filesize_str, strlen(filesize_str), 0);
    
    printf("Uploading...\n");
    FILE *p_file = fopen(file_complete_path, "rb"); 
    if (p_file == NULL) {
        ERROR_VERBOSE_LOG("Invalid file pointer\n");
        free(filename);
        return -1;
    }

    // Connect to the data transference port (emulated by 8020)
    // UniSocket_t *p_data_socket_t = create_socket_struct(false, DATA_PORT, is_ipv4(FTP_ADDR_IPV6), FTP_ADDR_IPV6);
    // if (p_data_socket_t == NULL) {
    //     ERROR_VERBOSE_LOG("Error creating socket for the client for the desired service");
    //     return -1;
    // }
    // INFO_VERBOSE_LOG("Client socket created successfully\n");

    off_t remaining_to_send = filesize; //TODO use the other port
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

    // Receive the server confirmation of the file transference success
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    fill_buffer_with_response(p_client_t);
    printf("%s", p_client_t->buffer);
    fflush(stdout); // allow the server to display the full content of its message

    free(filename);
    return 0;
}

//TODO compact the file before the transference ("Those units have compression applied to them")
int compact_file() { 

}

int list_available_files() 
{ 
    printf("Assets' folder structure:\n");

    DIR *directory;
    struct dirent *entry;
    directory = opendir(PATH_ASSETS_FOLDER);
    if (directory)
    {
        int file_idx = 0;
        while ((entry = readdir(directory)) != NULL)
        {
            printf("(%d) %s\n", file_idx, entry->d_name);
            file_idx++;
        }
        printf("\n");
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

int keep_connection_with_server_cli(ClientInfo_t *p_client_t)
{ 
    //  Prepare the structure to maintain the client-server interaction
    fd_set comm_sets;
    FD_ZERO(&comm_sets);
    FD_SET(0, &comm_sets); // stdin (client input)
    FD_SET(p_client_t->sock_FD, &comm_sets);

    while (true)
    {
        memset(p_client_t->buffer, 0, BUFFER_SIZE);
        fd_set curr_set = comm_sets;
        if (select(p_client_t->sock_FD + 1, &curr_set, NULL, NULL, NULL) < 0)
        { // +1 to include the check for the client socket
            ERROR_VERBOSE_LOG("Error when selecting the descriptors");
            return -1;
        }

        if (FD_ISSET(0, &curr_set)) // the client sent something
        {
            
            if (fgets(p_client_t->buffer, BUFFER_SIZE, stdin) == NULL)
            {
                ERROR_VERBOSE_LOG("Error reading input from the client (stdin)");
                return -2;
            }

            // Sanitize the input
            size_t len_str = strlen(p_client_t->buffer);
            if (len_str > 0 && p_client_t->buffer[len_str - 1] == '\n') {
                p_client_t->buffer[len_str - 1] = '\0';
            }

            // Interpret commands to anticipate needs from the client side
            if (strcmp(p_client_t->buffer, CMD_UPLOAD_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_UPLOAD_FULL) == 0)
            { 
                upload_file(p_client_t);
            }
            else if (strcmp(p_client_t->buffer, CMD_DOWNLOAD_SHORT) == 0 || strcmp(p_client_t->buffer, CMD_DOWNLOAD_FULL) == 0)
            { 
                download_file(p_client_t);
            }
            else
            { // Normally respond to the server
                send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0);
            }
        }
        else if (FD_ISSET(p_client_t->sock_FD, &curr_set))
        {
            // Get the server's content
            int num_bytes = recv(p_client_t->sock_FD, p_client_t->buffer, BUFFER_SIZE, 0);
            if (num_bytes < 0)
            {
                ERROR_VERBOSE_LOG("Error receiving the contents from the server\n");
                return -3;
            }
            else if (num_bytes == 0)
            {
                printf("Connection terminated by the server\n");
                return 0;
            }
            p_client_t->buffer[num_bytes] = '\0';
            printf("%s", p_client_t->buffer);
            fflush(stdout); // allows the multi-line server output to be entirely displayed
        }
    }

    return 0;
}

int start_using_FTP(ClientInfo_t *p_client_t)
{
    if (p_client_t == NULL)
    {
        perror("Invalid pointer to the client struct");
        return -1;
    }

    int connection_status;
    if ((connection_status = keep_connection_with_server_cli(p_client_t)) < 0)
    {
        perror("Error at some point during the connection");
        return connection_status;
    }

    return 0;
}
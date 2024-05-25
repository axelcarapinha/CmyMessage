#include "ftp_client.h"

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
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

        if (FD_ISSET(0, &curr_set))
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

            // Interpretate commands to anticipate needs from the client side
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

    printf("Downloading the file...\n");
    off_t remaining_to_recv = filesize;
    do
    {   
        // Keep track of the amount received
        off_t amount_to_recv;
        if (remaining_to_recv - BUFFER_SIZE >= 0) {
            amount_to_recv = BUFFER_SIZE;
            remaining_to_recv -= BUFFER_SIZE;
        }
        else {
            amount_to_recv = remaining_to_recv;
            remaining_to_recv = 0;
        }

        // Read the content from the socket
        if (fill_buffer_with_response(p_client_t) < 0) {
            printf("Error while dowloading the remaining part of the file");
        }
        fwrite(p_client_t->buffer, 1, amount_to_recv, p_file);

    } while (remaining_to_recv > 0);
    printf("File downloaded!\n");    

    // Ensure the changes to the file get written
    fclose(p_file);

    free((void *)filename);
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    return 0;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
int upload_file(ClientInfo_t *p_client_t)
{
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    send(p_client_t->sock_FD, CMD_UPLOAD_FULL, strlen(CMD_UPLOAD_FULL), 0);

    list_available_files(); // on this host

    printf("File to upload: ");
    if (fgets(p_client_t->buffer, BUFFER_SIZE, stdin) == NULL)
    {
        ERROR_VERBOSE_LOG("Error getting the pretended filename to upload");
        return -1;
    }

    char *filename = strdup(p_client_t->buffer);
    if (strlen(filename) > MAX_LEN_FILE_PATH) {
        printf("File path is too long. Please, provide a shorter one.");
    }
    filename[strlen(filename) - 1] = '\0'; // remove the newline

    char *file_complete_path = (char *)malloc(MAX_LEN_FILE_PATH);
    strcpy(file_complete_path, PATH_ASSETS_FOLDER);
    strcat(file_complete_path, filename);

    long filesize = get_file_size(file_complete_path);
    if (filesize < 0) {
        printf("Error calculating the file's size.\n");
        printf("Please, try again.\n");
        return 0;
    }
    else if (filesize > MAX_FILE_SIZE)
    {
        printf("That file is enormous! This server cannot handle it for now\n");
        printf("Sorry. Please, try another one\n");
        return 0;
    }

    // Allow the server to prepare the receiving file
    send(p_client_t->sock_FD, filename, strlen(filename), 0);
    //
    char filesize_str[MAX_NUM_ALGS_FILESIZE]; 
    snprintf(filesize_str, sizeof(filesize_str), "%d", filesize); // more robust than (char) typecaster
    send(p_client_t->sock_FD, filesize_str, strlen(filesize_str), 0);
    
    printf("Uploading...\n");
    FILE *p_file = fopen(file_complete_path, "rb"); 
    if (p_file == NULL) {
        ERROR_VERBOSE_LOG("Invalid file pointer");
    }
    off_t amount_sent;
    memset(p_client_t->buffer, 0, BUFFER_SIZE);
    while (fgets(p_client_t->buffer, BUFFER_SIZE, p_file) != NULL) {
        send(p_client_t->sock_FD, p_client_t->buffer, strlen(p_client_t->buffer), 0);
    }
    memset(p_client_t->buffer, 0, BUFFER_SIZE);

    // Confirm the success of the transference
    fill_buffer_with_response(p_client_t);
    printf("%s", p_client_t->buffer);

    return 0;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
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

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @return int
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
    fclose(p_file); //TODO valgrind testing
    
    return size;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief

 * @param
 *
 * @return
 */
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
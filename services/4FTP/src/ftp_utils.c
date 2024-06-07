#include "ftp_utils.h"

off_t get_file_size(const char *file_complete_path) // Support all systems (NOT only POSIX respecting ones)
{
    struct stat file_stat; // file's status
    off_t size;

    if (access(file_complete_path, F_OK) != 0) {
        ERROR_VERBOSE_LOG("File does NOT exist.\n");
        return -1;
    }
    else if (strlen(file_complete_path) > (MAX_LEN_FILE_PATH - 1)) {
        ERROR_VERBOSE_LOG("File path is too long\n");
        return -2;
    }
    else if (stat(file_complete_path, &file_stat) == -1)
    {
        ERROR_VERBOSE_LOG("Invalid path for the file\n");
        return -3; 
    }

    FILE *p_file = fopen(file_complete_path, "rb"); //TODO name the file pointers this way everywhere
    if (p_file == NULL) {
        ERROR_VERBOSE_LOG("Error opening the file");
        return -1;
    }
    fseek(p_file, 0, SEEK_END); 
    size = ftell(p_file); 
    fclose(p_file); 
    
    return size;
}

int receive_file(ClientInfo_t *p_client_t, FILE *p_file, char *filename, int filesize) {
    int remaining_to_recv = filesize;
    while (remaining_to_recv > 0) {
        size_t amount_to_recv = remaining_to_recv > BUFFER_SIZE ? BUFFER_SIZE : remaining_to_recv;
        ssize_t bytes_received = recv(p_client_t->sock_FD, p_client_t->buffer, amount_to_recv, 0);
        if (bytes_received < 0) {
            ERROR_VERBOSE_LOG("Error receiving the file content");
            fclose(p_file);
            free(filename);
            return -1;
        }
        fwrite(p_client_t->buffer, 1, bytes_received, p_file);
        remaining_to_recv -= bytes_received;
    }
    fclose(p_file);
}

int send_file(ClientInfo_t *p_client_t, FILE *p_file, char *filename, int filesize) {
    off_t remaining_to_send = filesize;
    while (remaining_to_send > 0)
    {
        size_t amount_to_send = remaining_to_send > BUFFER_SIZE ? BUFFER_SIZE : remaining_to_send;
        size_t bytes_read = fread(p_client_t->buffer, 1, amount_to_send, p_file);
        if (bytes_read <= 0) {
            ERROR_VERBOSE_LOG("Error reading content from the file to upload");
            fclose(p_file);
            free(filename);
            return -1;
        }
        send(p_client_t->sock_FD, p_client_t->buffer, bytes_read, 0);
        remaining_to_send -= bytes_read;
    }
    fclose(p_file);
}

//TODO learn more about compression and implement a good algorithm
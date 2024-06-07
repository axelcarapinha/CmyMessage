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

int receive_content() {

}

int send_content() {

}

int compact_file() {
    
}
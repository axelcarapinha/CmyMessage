#ifndef FTP_SERVICE_H // passive mode
#define FTP_SERVICE_H

#include <pthread.h>
#include <semaphore.h>
#include <asm-generic/socket.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
//
#include "service_utils.h"
#include "clients.h"

// Settings
#define SERVICE_PORT 8021
#define CONTENT_PORT 8021
#define MAX_SIZE_USER_OPTION 1 // a single number (for the option)
#define MAX_SIZE_ADDR_INFO 200
#define MAX_LEN_FILE_PATH 1000 
#define MAX_NUM_ALGS_FILESIZE 20


//
//TODO use the same in the client side too
#define MAX_FILE_SIZE (1024 * 1024 * 1000) // 1'000 megabytes
//
#define SERVICE_NAME "FTP"
//
#define CLIENT_DISCONNECTED 404

// Utilities
typedef enum {
    UPLOAD,
    DOWNLOAD,
    HELP,
    INVALID_OPTION
} Options;

#define CMD_UPLOAD_SHORT "-u"
#define CMD_UPLOAD_FULL "--upload"
//
#define CMD_HELP_SHORT "-h"
#define CMD_HELP_FULL "--help"
//
#define CMD_DOWNLOAD_SHORT "-d"
#define CMD_DOWNLOAD_FULL "--download"
//
#define CMD_LIST_SHORT "-l"
#define CMD_LIST_FULL "--list"
//
#define CMD_EXIT_FULL "--exit"

#define PATH_ASSETS_FOLDER "assets/" //TODO call it "path" instead of name

//TODO place the comments in the header file
int list_files_curr_dir(ClientInfo_t *p_client_t);
int download_file(ClientInfo_t *p_client_t);
int upload_file(ClientInfo_t *p_client_t);
int inform_client(ClientInfo_t *p_client_t);
off_t get_file_size(const char *file_complete_path);
int input_client_commands(ClientInfo_t *p_client_t);
int ask_client_basic_details(ClientInfo_t *p_client_t);
void *prepare_client_structs_for_data(ClientInfo_t *p_client_t);
int serve_client_with_FTP(ClientInfo_t *p_client_t);

#endif
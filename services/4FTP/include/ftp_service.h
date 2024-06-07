#ifndef FTP_SERVICE_H // passive mode (the client connects to the server for the file transference)
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

#define VERBOSE true

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
#define SERVICE_NAME "FTP"
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

#define PATH_ASSETS_FOLDER "assets/" 

/**
 * @brief Outputs the files of the asset's folder
 * @param p_client_t (struct with the data of the client)
 */
int list_files_curr_dir(ClientInfo_t *p_client_t);


/**
 * @brief Outputs the files of the asset's folder
 * @param p_client_t (struct with the data of the client)
 * @return int (exit status)
 */
int download_file(ClientInfo_t *p_client_t);

/**
 * @brief Uploads the file from the client side
 * @param p_client_t (struct with the data of the client)
 * @return int (exit status)
 */
int upload_file(ClientInfo_t *p_client_t);
int inform_client(ClientInfo_t *p_client_t);

/**
 * @brief Calculates the size of the file
 * @param file_complete_path (path to the file itself)
 * @return off_t (size of the file)
 */
off_t get_file_size(const char *file_complete_path);


/**
 * @brief Presents the FTP service
 * and redirects the user to the desired function
 * based on the command 
 * @param p_client_t (struct with the data of the client)
 * @return int (exit status)
 */
int input_client_commands(ClientInfo_t *p_client_t);

/**
 * @brief Asks the client basic infos
 * and stores them in the client structure
 * @param p_client_t (struct with the data of the client)
 * @return int (exit status)
 */
int ask_client_basic_details(ClientInfo_t *p_client_t);

/**
 * @brief Allocates the memory for the client struct 
 * (that holds the informations)
 * @param p_client_t (struct with the data of the client)
 */
void *prepare_client_structs_for_data(ClientInfo_t *p_client_t);

int serve_client_with_FTP(ClientInfo_t *p_client_t);


/**
 * @brief Starting point to the service, 
 * being the function that is started from the main.c file
 * @param p_client_t
 * @return
 */
int serve_client_with_FTP(ClientInfo_t *p_client_t)



//TODO
/**
 * @brief 
 * @param
 * @return
 */
// UniSocket_t *get_transference_socket();


#endif
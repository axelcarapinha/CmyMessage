#ifndef FTP_SERVICE_H // passive mode (the client connects to the server for the file transference)
#define FTP_SERVICE_H

#include <pthread.h>
#include <semaphore.h>
#include <asm-generic/socket.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
//
#include "ftp_utils.h"
#include "service_utils.h"
#include "clients.h"

#define VERBOSE true

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

/**
 * @brief Sends the available "FTP-like" commands to the client
 * @param p_client_t (struct with the data of the client)
 * @return int (exit status)
 */
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

/**
 * @brief Starting point of the service, 
 * being the function that is executed from the main.c file
 * @param p_client_t
 * @return
 */
int serve_client_with_FTP(ClientInfo_t *p_client_t);

#endif
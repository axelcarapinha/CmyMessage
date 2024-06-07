/*
 * Purpose: FTP abstractions for the CLIENT side
 */

#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "ftp_utils.h"

#define PATH_ASSETS_FOLDER "assets/" // CLIENT side (may be different from the SERVER side)


/**
 * @brief Download the desired file,
 * from the available in the asset's folder of the server
 * @param p_client_t (pointer to the client' information struct)
 * @return int (exit status)
 */
int download_file(ClientInfo_t *p_client_t);

/**
 * @brief Uploads one of the files from the 'assets' folder
 * after listing them, 
 * @param p_client_t (pointer to the client' information struct)
 * @return int (exit status)
 */
int upload_file(ClientInfo_t *p_client_t);

/**
 * @brief Lists the files of the 'assets' folder
 * @return int (exit status)
 */
int list_available_files();

/**
 * @brief Reads content from the sockets when possible,
 * and forces (flush) of stdout, in order to receive the multiline
 * content from the server
 * 
 * Also, interprets the commands and redirects to the desired function
 * @param p_client_t (pointer to the client' information struct)
 * @return int (exit status)
 */
int keep_connection_with_server_cli(ClientInfo_t *p_client_t);

/**
 * @brief Start point for the entire service
 * (function called from the client.c file)
 * 
 * @param p_client_t (pointer to the client' information struct)
 * 
 * @return int (exit status)
 */
int start_using_FTP(ClientInfo_t *p_client_t);

#endif
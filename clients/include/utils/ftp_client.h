/*
 * Purpose: Allow to configure the service behaviour and settings,
 * while supplying the implmentation of an "FTP-like" server
 * (please, reference the README.md for more details)
 */

#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "net_utils_tcp.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PATH_ASSETS_FOLDER "assets/"


#define DATA_PORT 8020
#define CONTROL_PORT 8021



#define MAX_FILE_SIZE (1024 * 1024 * 10 * 1000LL) // 10'000 megabytes
#define MAX_LEN_FILE_PATH 1000 
#define MAX_NUM_ALGS_FILESIZE 20



#define CMD_UPLOAD_SHORT "-u"
#define CMD_UPLOAD_FULL "--upload"
//
#define CMD_DOWNLOAD_SHORT "-d"
#define CMD_DOWNLOAD_FULL "--download"
//
#define CMD_LIST_SHORT "-l"
#define CMD_LIST_FULL "--list"

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
 * @brief
 * @return
 */
int compact_file(); //TODO

/**
 * @brief Lists the files of the 'assets' folder
 * @return int (exit status)
 */
int list_available_files();

/**
 * @brief Calculates the size of the file in bytes
 * @param file_complete_path (path to the file, considering the one
 * to the assets folder)
 * @return off_t (filesize, in bytes)
 */
off_t get_file_size(const char *filename);

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
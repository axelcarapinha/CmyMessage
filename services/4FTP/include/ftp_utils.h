/*
 * Purpose: Allows to configure the service behaviour and settings,
 * while supplying generalized implementations
 * for more abstracted use of an "FTP-like" server
 */

#ifndef FTP_UTILS_H
#define FTP_UTILS_H

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
//
#include "net_utils_tcp.h"

// Configurations
#define SERVICE_NAME "FTP"
#define FTP_ADDR_IPV4 "127.0.0.1" 
#define FTP_ADDR_IPV6 "::1"
//
#define DATA_PORT 8020
#define CONTROL_PORT 8021
//
#define MAX_FILE_SIZE (1024 * 1024 * 10 * 1000LL) // 10'000 megabytes
#define MAX_LEN_FILE_PATH 1000 
#define MAX_NUM_ALGS_FILESIZE 20
#define CLIENT_DISCONNECTED 404
//
#define CMD_UPLOAD_SHORT "-u"
#define CMD_UPLOAD_FULL "--upload"
//
#define CMD_DOWNLOAD_SHORT "-d"
#define CMD_DOWNLOAD_FULL "--download"
//
#define CMD_LIST_SHORT "-l"
#define CMD_LIST_FULL "--list"
//
#define CMD_HELP_SHORT "-h"
#define CMD_HELP_FULL "--help"
//
#define CMD_EXIT_FULL "--exit"

/**
 * @brief Calculates the size of the file in bytes
 * @param file_complete_path (path to the file, considering the one
 * to the assets folder)
 * @return off_t (filesize, in bytes)
 */
off_t get_file_size(const char *filename);

/**
 * @brief RECEIVES the content of the file
 * controlling the amount RECEIVED with its size
 * 
 * Thought of implementing a few 'if' statements
 * in order to merge with the send_file(),
 * BUT this is more:
 *  - explicit
 *  - efficient
 *  - potentially mantainable (if needing to add more features in the future)
 * 
 * @param p_client_t 
 * @param p_file 
 * @param filename 
 * @param filesize 
 * @return int 
 */
int receive_file(ClientInfo_t *p_client_t, FILE *p_file, char *filename, int filesize);

/**
 * @brief SENDS the content of the file
 * controlling the amount SENT with its size
 * 
 * Thought of implementing a few 'if' statements
 * in order to merge with the receive_file(),
 * BUT this is more:
 *  - explicit
 *  - efficient
 *  - potentially mantainable (if needing to add more features in the future)
 * 
 * @param p_client_t 
 * @param p_file 
 * @param filename 
 * @param filesize 
 * @return int 
 */
int send_file(ClientInfo_t *p_client_t, FILE *p_file, char *filename, int filesize);


#endif
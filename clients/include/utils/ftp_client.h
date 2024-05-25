#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "clients.h"
//
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

//TODO sync with the server too (with config file, consider)
#define MAX_FILE_SIZE (1024 * 1024 * 10 * 1000) // 10'000 megabytes
#define PATH_ASSETS_FOLDER "assets/"
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

int keep_connection_with_server_cli(ClientInfo_t *p_client_t);
int upload_file(ClientInfo_t *p_client_t);
int list_available_files();
off_t get_file_size(const char *filename);
int start_using_FTP(ClientInfo_t *p_client_t);

#endif
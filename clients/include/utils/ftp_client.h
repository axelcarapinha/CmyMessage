#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "clients.h"

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
#define CMD_DOWNLOAD_FULL "--full"


int start_using_FTP(ClientInfo_t *p_client_t);
int keep_connection_with_server_cli(ClientInfo_t *p_client_t);

#endif
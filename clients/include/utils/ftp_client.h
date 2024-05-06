#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "clients.h"

//TODO sync with the server too (with config file, consider)
#define MAX_FILE_SIZE (1024 * 1024 * 10 * 1000) // 10'000 megabytes

int start_using_FTP(ClientInfo_t *p_client_t);
int keep_connection_with_server_cli(ClientInfo_t *p_client_t);

#endif
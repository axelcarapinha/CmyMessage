#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "clients.h"

int start_using_FTP(ClientInfo_t *p_client_t);
int keep_connection_with_server_cli(ClientInfo_t *p_client_t);

#endif
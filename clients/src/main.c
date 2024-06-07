#include "clients.h"
#include "ftp_client.h"

int main(int argc, char *argv[])
{
    if (VERBOSE) {
        printf(YELLOW "Connecting to the FTP server!\n" RESET);
    }

    // Connect to the desired server (just by changing the macro FTP_ADDR_IPV6)
    // The PASV command is assumed by default (in this project)
    int exit_status; 
    if ((exit_status = use_service(CONTROL_PORT, FTP_ADDR_IPV6, start_using_FTP)) < 0) {
        ERROR_VERBOSE_LOG("Failed to use the desired service");
    }

    return 0;
}
#include "ftp_service.h"

int main(int argc, char *argv[])
{
    if (VERBOSE) {
        printf(YELLOW "Powering up the service %s!\n" RESET, SERVICE_NAME);
    }

    // Starts the function sent by argument in the SERVICE_PORT
    int exit_status;
    if ((exit_status = start_service(CONTROL_PORT, FTP_ADDR_IPV6, serve_client_with_FTP)) < 0) {
        ERROR_VERBOSE_LOG("Failed to start the FTP server");
    }

    return 0;
}
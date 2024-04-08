#include "ftp_service.h"

int main(int argc, char *argv[])
{
    if (VERBOSE) {
        printf(YELLOW "Powering up the service %s!\n" RESET, SERVICE_NAME);
    }

    // Start the BROADCASTING service
    int exit_status;
    if ((exit_status = start_service(DEFAULT_PORT, serve_client_with_FTP)) < 0) {
        ERROR_VERBOSE_LOG("Failed to start the FTP server");
    }

    return 0;
}
#include "clients.h"
//
#include "ftp_client.h"

int main(int argc, char *argv[])
{
    if (VERBOSE) {
        printf(YELLOW "Connecting to the FTP server!\n" RESET);
    }

    // Connect to the desired server
    int exit_status; //TODO mudar para IPv6 (lembrar de mudar o boolean também, o que conectas as coisas)
    if ((exit_status = use_service(FTP_COMMANDS_PORT, FTP_ADDR_IPV6, start_using_FTP)) < 0) {
        ERROR_VERBOSE_LOG("Failed to use the desired service");
    }

    return 0;
}
#include "service_utils.h"
#include "broadcast_chat_service.h"

int main(int argc, char *argv[])
{
    if (VERBOSE) {
        printf(YELLOW "Powering up the service %s!\n" RESET, SERVICE_NAME);
    }

    // Broadcasting service
    int exit_status;
    if ((exit_status = start_service(DEFAULT_PORT, prepare_client_for_broadcast_and_start)) < 0) {
        ERROR_VERBOSE_LOG("Failed to start the broadcasting service");
    }

    return 0;
}
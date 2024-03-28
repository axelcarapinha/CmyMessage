#include "service_utils.h"
#include "broadcast_chat_service.h"

int main(int argc, char *argv[])
{
    puts(YELLOW "Powering up the service!" RESET);
    start_service(DEFAULT_PORT, prepare_client_for_broadcast_and_start);

    //TODO handle error cases (provavelmente criar uma thread)
    // int status;
    // if ((status = start_service(PORT)) < 0) {
    //     fprintf(stderr, "Error number %d when starting the server.\n", status);
    // }

    return 0;
}
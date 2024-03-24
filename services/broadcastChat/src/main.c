#include "service_utils.h"
#include "client_handler_service.h"

int main(int argc, char *argv[])
{
    puts(YELLOW "Powering up the service!" RESET);
    start_service(DEFAULT_PORT, handle_client_requests);


    //TODO handle error cases
    // int status;
    // if ((status = start_service(PORT)) < 0) {
    //     fprintf(stderr, "Error number %d when starting the server.\n", status);
    // }

    return 0;
}
#include "service_utils.h"
#include "broadcast_chat_service.h"

int main(int argc, char *argv[])
{
    printf(YELLOW "Powering up the service %s!\n" RESET, SERVICE_NAME);
    start_service(DEFAULT_PORT, prepare_client_for_broadcast_and_start);
    
    // Connect to the database

    return 0;
}
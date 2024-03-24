#include <stdio.h>
#include <stdlib.h>

//! "base 64 is largely used in C"

int main(int argc, char *argv[])
{
    check_cli_access_options();
    

    return 0;
}


void login_client(long *client_handler_ptr, char *buffer)
{
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, "Username: ");
    send(*client_handler_ptr, buffer, BUFFER_SIZE, 0);

    // send('\n')
}

void register_client(long *client_handler_ptr, char *buffer)
{
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, "Pretended Username: ");
    send(*client_handler_ptr, buffer, BUFFER_SIZE, 0);
}

void logout_client(long *client_handler_ptr, char *buffer)
{
    // ClientInfo *recipient = (ClientInfo *)malloc(sizeof(ClientInfo));

    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, "You logged out. Have a nice day :)\n");
    send(*client_handler_ptr, buffer, BUFFER_SIZE, 0);
}





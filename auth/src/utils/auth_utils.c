#include "auth_utils.h"

void enter_cli_as_guest()
{
    puts("Guest mode");
}

void login_client()
{
    puts("Login mode");
}

void logout_client()
{
    puts("Logout");
}

void register_client()
{
    puts("Register mode");

    return;

    char email[MAX_EMAIL_ADDR_SIZE];
    char username[MAX_USERNAME_SIZE];

    puts("Ok. Let's create an account...");
    printf("Email: ");
    fgets(email, MAX_EMAIL_ADDR_SIZE + 1, stdin);
    // TODO check for validity of the input

    printf("Username: ");
    fgets(username, MAX_USERNAME_SIZE + 1, stdin);
    // TODO check for the validity of the input
}

void check_cli_access_options()
{
    // Understand the need of the client
    char *buffer[BUFFER_SIZE];
    //
    printf("Welcome!\n");
    printf("(%d) Login\n", LOGIN);
    printf("(%d) Register\n", REGISTER);
    printf("(%d) Logout\n", LOGOUT);
    printf("(%d) Guest\n", GUEST);
    printf("Enter the number of the pretended option: \n");
    fgets(buffer, 2, stdin); // null character + user_input
    //
    bool valid_user_input = true;
    int opt = buffer[0];
    do
    {
        switch (opt)
        {
        case LOGIN:
            login_client();
            break;
        case REGISTER:
            register_client();
            break;
        case LOGOUT:
            logout_client();
            break;
        case GUEST:
            enter_cli_as_guest();
            break;
        default:
            printf("Invalid option. Please, try again.");
            break;
        }
    } while (valid_user_input == false);
}
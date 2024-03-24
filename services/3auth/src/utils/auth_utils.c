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
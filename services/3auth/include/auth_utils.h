#ifndef AUTH_UTILS_H
#define AUTH_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


// RFC 5321 and RFC 5322 (respecting that standards)
// 64 (local part) + 1 (@) +255 (domain part)
#define MAX_EMAIL_ADDR_SIZE 320
#define MAX_USERNAME_SIZE 150
#define PASSWORD_MAX_SIZE 150 

#define BUFFER_SIZE 1024

// Utils
void enter_cli_as_guest();
void login_client();
void logout_client();
void register_client();
void check_cli_access_options();

#endif
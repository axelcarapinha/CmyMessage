#include <stdio.h>

int main(int argc, char *argv[])
{
    // scanf_testing();
    is_digit_testing();

    return 0;
}

void scanf_testing() {
    char buffer[10];
    int v = 0;

    puts("What's your name: ");
    gets(buffer);
    // scanf("%s", buffer);
    printf("Hello %s! \n", buffer);
    printf("v = %d | v = %x\n", v, v);
}

void is_digit_testing() {
    int value = 1;
    if (isdigit(value)) {
        puts("YAY");
    }
    else {
        puts("NAY");
    }
    puts("-------------");


    char *word = "1234";
    if (isdigit(word[0])) {
        puts("YAY");
    }
    else {
        puts("NAY");
    }
    puts("-------------");


    char *another_word = "1234";
    if (isdigit((int)another_word[0])) {
        puts("YAY");
    }
    else {
        puts("NAY");
    }
}
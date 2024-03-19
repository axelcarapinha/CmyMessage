#include <stdio.h>

int main(int argc, char *argv[])
{
    char buffer[10];
    int v = 0;

    puts("What's your name: ");
    gets(buffer);
    // scanf("%s", buffer);
    printf("Hello %s! \n", buffer);
    printf("v = %d | v = %x\n", v, v);

        return 0;
}
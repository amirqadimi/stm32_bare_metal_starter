#include <stdio.h>

int main(int argc, char *argv[])
{
    int a = 31;
    if (argc < 2)
    {
        printf("Please specify arguments\n");
    }

    printf("Argc: %d\n", a);

    for (int i = 1; i < argc; i++)
    {
        printf("%s\n", argv[i]);
    }

    return 0;
}
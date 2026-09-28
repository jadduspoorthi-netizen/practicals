#include <stdio.h>
#include <stdlib.h>

int global_var = 10;
int global_uninit;

void display_code_address(void)
{
    printf("Code address : %p\n", (void *)display_code_address);
}

int main(void)
{
    static int static_var = 20;
    int stack_var = 30;
    int *heap_var = (int *)malloc(sizeof(int));

    if (heap_var == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    *heap_var = 40;

    printf("===== Linux Process Address Space =====\n\n");

    display_code_address();

    printf("Global address : %p\n", (void *)&global_var);
    printf("Static address : %p\n", (void *)&static_var);
    printf("BSS address : %p\n", (void *)&global_uninit);
    printf("Heap address : %p\n", (void *)heap_var);
    printf("Stack address : %p\n", (void *)&stack_var);

    free(heap_var);

    return 0;
}

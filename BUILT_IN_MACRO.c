#include <stdio.h>

void demo_function() {
    printf("Function: %s\n", __func__);
    printf("File: %s\n", __FILE__);
    printf("Line: %d\n", __LINE__);
}

int main() {
    printf("Compilation Date: %s\n", __DATE__);
    printf("Compilation Time: %s\n", __TIME__);

    printf("Is Standard C? %d\n", __STDC__);
    #ifdef __STDC_VERSION__
        printf("C Standard Version: %ld\n", __STDC_VERSION__);
    #endif
    printf("Hosted Environment? %d\n", __STDC_HOSTED__);

    demo_function();

    printf("You are reading this at line: %d\n", __LINE__);

    return 0;
}

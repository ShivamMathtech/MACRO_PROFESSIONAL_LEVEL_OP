#include <stdio.h>

#define TRACE() printf("TRACE: %s() called at %s:%d\n", __func__, __FILE__, __LINE__)

void hello() {
    TRACE();
    printf("Hello World!\n");
}

int add(int a, int b) {
    TRACE();
    return a + b;
}

int main() {
    TRACE();
    hello();
    int result = add(5, 7);
    printf("Result: %d\n", result);
    return 0;
}

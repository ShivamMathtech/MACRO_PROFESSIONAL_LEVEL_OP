#include <stdio.h>

#define SWAP(a, b) \
    do {           \
        int temp = a; \
        a = b;     \
        b = temp;  \
    } while (0)

int main() {
    int x = 5, y = 10;

    printf("Before swap: x=%d, y=%d\n", x, y);

    SWAP(x, y); // macro call

    printf("After swap:  x=%d, y=%d\n", x, y);

    return 0;
}

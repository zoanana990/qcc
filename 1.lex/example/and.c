#include <stdio.h>

int and(int a, int b) {
    return a && b;
}

int main() {
    int a = 5, b = 13;
    printf("%d && %d = %d\n", a, b, and(a, b));
    return 0;
}
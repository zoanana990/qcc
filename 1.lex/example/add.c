#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int main() {
    int a = 5, b = 13;
    printf("%d + %d = %d\n", a, b, add(a, b));
    return 0;
}
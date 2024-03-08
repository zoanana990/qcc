#include <stdio.h>

int x;

struct x{
    int x;
    int y;
};

int add(int x, int y) {
    return x + y;
}

int main() {
    int x = 1;
    int sum;
    {
        int x = 2;
        {
            int x = 3;
            sum = add(x, 1);
        }
    }
    printf("sum = %d\n", sum);
    return 0;
}
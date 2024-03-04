# LL(1)

```c
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void E();
void E_prime();
void T();
void T_prime();
void F();

char input[] = "id + id * id";

int current = 0;

char parsingTable[3][5] = {
    {'T', 'E', 'T', 'E', 'ε'},
    {'+', 'ε', 'ε', 'ε', 'ε'},
    {'F', 'T', 'F', 'T', 'ε'}
};

void error() {
    printf("Error: Invalid expression.\n");
    exit(1);
}

void match(char terminal) {
    if (input[current] == terminal) {
        current++;
    } else {
        error();
    }
}

void E() {
    printf("E -> TE'\n");
    T();
    E_prime();
}

void E_prime() {
    char terminal = input[current];
    int row = 0;

    if (isalpha(terminal)) {
        row = 2;
    } else if (terminal == '+') {
        row = 0;
    } else {
        row = 1;
    }

    printf("E' -> %c\n", parsingTable[1][row]);
    match(parsingTable[1][row]);

    if (terminal == '+' || isalpha(terminal)) {
        T();
        E_prime();
    }
}

void T() {
    printf("T -> FT'\n");
    F();
    T_prime();
}

void T_prime() {
    char terminal = input[current];
    int row = 0;

    if (isalpha(terminal)) {
        row = 2;
    } else if (terminal == '*') {
        row = 0;
    } else {
        row = 1;
    }

    printf("T' -> %c\n", parsingTable[3][row]);
    match(parsingTable[3][row]);

    if (terminal == '*' || isalpha(terminal)) {
        F();
        T_prime();
    }
}

void F() {
    char terminal = input[current];
    int row = 0;

    if (terminal == '(') {
        printf("F -> (E)\n");
        match('(');
        E();
        match(')');
    } else if (isalpha(terminal)) {
        printf("F -> id\n");
        match('i');
        match('d');
    } else {
        error();
    }
}

int main() {
    E();
    printf("Parsing successful.\n");

    return 0;
}
```
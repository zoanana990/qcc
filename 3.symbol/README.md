# Symbol Table
This project is used to highlight c programming language syntax

## Introduction
In this project, we will implement the symbol table and some semantics check.
Corresponding symbol checks can be performed through the context of the symbol table

Case 1: variable need to be declared before using
```c
int A() {
    x = 1;
}
```
Here, we need to raise a compile error, symbol `x` need to be declared

Case 2: the array name cannot be lvalue 
```c
char A[8];
A = 1;
```

## Implementation

How do we implement the `symbol table`?
The answer is `stack`.

Please see the following code.


## Compile

```shell
make all
```

## Start

```shell
make check
```

## clean
```shell
make clean
```

## Development record

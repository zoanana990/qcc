# Specification
`qcc` is a simplified c compiler from c programming language.

We support most of [`c99`](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf) features, but some of them we will ignored it, or implement it in the future

## Prerequisite
The format we use is [`EBNF`](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form), extended backus-Naur form, which is a family of metasyntax notations, and of which can be used to express a context-free grammer.

### syntax of EBNF
| Usage                | Notation |
|----------------------|----------|
| Definition           | `::=`    |
| Alternation          | `\|`     |
| Termination          | `.`      |
| Grouping             | `()`     |
| Optional             | `[]`     |
| Repeating            | `{}`     |
| Terminal string      | `""`     |
| Non-terminal string  | `<>`     |

## Parsing Method

### LL(1)
LL parser (Left to rignt, leftmost derivation), is a top-down parser for a restricted contex-free language.


We use LL(1) as our parsing table. 

### First Set

### Follow Set

### Select Set

## Features


## De
# Specification
`qcc` is a simplified c compiler from c programming language.

We support most of [`c89`]() features, but some of them we will ignored it, or implement it in the future

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


## Features

# Compiler
This is a simple compiler for mathematical formulas.

First write some variables as `a = 179`, separate initializations with semicolon.

Then write `return <expression>` and write some mathematical expression using `+`, `-`, `*`, `/`, `()`. You can also write such expressions in variable initialization or association.

## Simple example
Input:
`a = 1 + 2;
b = 1 - a * 3 + 7;
c = a + b + a - 1;
a = c - b;
return c * 3 - b * 2 + -1 * a`

Output: 9

Comment:
a = 3, b = -1, c = 4, a = 5, output = 4 * 3 + 1 * 2 - 5 = 9

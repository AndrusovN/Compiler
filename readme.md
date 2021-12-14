# Compiler
This is a simple compiler for mathematical formulas.

Now it supports only `+` operator. So you can assign variables, calculate sum of them and log values.
# Example
	1. `(a = 1);` -- assignation needs brackets around it,
	2. `(b = a + 7 + (11 + 1));` -- you can use variables to set values of new variables,
	3. `<< a + b + 1 >>;` -- this will print to console value of `a + b + 1`
	4. `(a = 1); (b = a + 7 + (11 + 1)); << a + b + 1 >>; << (a = a + b) >>;` -- this will print 22 and 21 to console

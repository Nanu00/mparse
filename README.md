# mparse
A simple math expression parser. Also a simple expression evaluator.

This was an exercise in learning C.

# How it works
Takes in a string and tokenizes it, making an array.

The array is then taken and shoved into a modified [Shunting-yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm) to make it into a binary tree instead of Reverse Polish notation.

The modified algorithm, instead of pushing tokens to an output stack, pushes binary tree nodes to the output stack and connecting them when needed. This builds a complete binary tree representation of the original expression with only the top node remaining in the output stack when the token array is completely consumed.

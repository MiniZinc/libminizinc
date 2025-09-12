/***
!Test
expected:
- !Result
  solution: !Solution
***/

test foo(tuple($T): x) = let {
    any: xx = x;
} in true;

any: x ::output_only = foo((3,));
output ["\(x)"];

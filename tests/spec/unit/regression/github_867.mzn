/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
- !Error
  type: SyntaxError
***/

% Rejected either way, but at a different stage depending on the parser: the
% bison grammar has a restricted sub-language for type domains that cannot
% contain `++`, so it reads this as a type concatenation and reports a type
% error. The tree-sitter grammar has no such restriction, so the `++` is
% absorbed into the domain expression and the item does not parse.
set of 1..2 ++ set of 2..3: no;

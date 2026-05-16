/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

% Multi-identifier generators (`i, j in S`) are not allowed in indexed
% array index lists; each dimension must be a separate `c in S' entry.
% Currently rejected at type-check time (the lone `i' parses as an index
% TypeInst whose domain is an unresolved identifier), giving a generic
% "undefined identifier" error rather than a dedicated syntax error.
array[i, j in 1..3] of var 1..(i+j): x;

solve satisfy;

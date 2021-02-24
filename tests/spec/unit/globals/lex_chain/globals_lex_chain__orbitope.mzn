/***
!Test
solvers: [gecode, cbc, scip]
expected:
- !Result
  solution: !Solution
    lex_members_int:
    - 1
    - 0
    - 1
    lex_members_bool:
    - false
    - false
***/

include "lex_chain_lesseq_orbitope.mzn";
include "lex_chain_greatereq_orbitope.mzn";

%-----------------------------------------------------------------------------%
% lex_chain_..._orbitope variants
%-----------------------------------------------------------------------------%

array[int, int] of var 0..1: arr1 = [|
   0, 0, _ |
   1, 1, 0 |
   0, 1, 0 |];
constraint lex_chain_lesseq_orbitope(arr1, 0);

array[int, int] of var bool: arr2 = [|
   false, false, true |
   _,     false, false|
   false, true,  _    |];
constraint lex_chain_lesseq_orbitope(arr2, 2);

array[int, int] of var 0..1: arr3 = [|
   1, 0, _ |
   0, 1, 0 |
   0, _, 0 |];
constraint lex_chain_greatereq_orbitope(arr3, 1);


solve satisfy;

output [
   "lex_members_int  = array1d(1..3, [\(arr1[1, 3]), \(arr3[1, 3]), \(arr3[3, 2])] );\n",
   "lex_members_bool = array1d(1..2, [\(arr2[2, 1]), \(arr2[3, 3])] );\n"
];

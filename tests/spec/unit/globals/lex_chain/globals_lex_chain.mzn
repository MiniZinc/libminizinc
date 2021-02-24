/***
!Test
expected:
- !Result
  solution: !Solution
    lex_members_int:
    - 6
    - 0
    - 0
    lex_members_bool:
    - false
***/

include "lex_chain_lesseq.mzn";
include "lex_chain_less.mzn";
include "lex_chain_greatereq.mzn";
include "lex_chain_greater.mzn";

%-----------------------------------------------------------------------------%
% lex_chain variants
%-----------------------------------------------------------------------------%

array[int, int] of var 0..6: arr1 = [|
   5, 5, _ |
   4, 4, 3 |
   2, 5, 3 |];
constraint lex_chain(arr1);

array[int, int] of var bool: arr2 = [|
   true,  true,  true |
   _,     true,  true |
   false, false, true |];
constraint lex_chain_less(arr2);

array[int, int] of var 0..6: arr3 = [|
   5, 1, _ |
   4, 4, 5 |
   2, 5, 5 |];
constraint lex_chain_greatereq(arr3);

array[int, int] of var 0..6: arr4 = [|
   5, 1, _ |
   4, 4, 4 |
   2, 5, 5 |];
constraint lex_chain_greater(arr4);

solve satisfy;

output [
   "lex_members_int  = array1d(1..3, [\(arr1[1, 3]), \(arr3[1, 3]), \(arr4[1, 3])] );\n",
   "lex_members_bool = array1d(1..1, ", show(arr2[2, 1]), ");\n"
];

/***
!Test
solvers: [gecode, cbc, highs]
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    x: [!Approx 1.0, null]
- !Result
  status: SATISFIED
  solution: !Solution
    x: [null, !Approx 1.0]
***/

%-----------------------------------------------------------------------------%
% This file tests the opt float vars
%-----------------------------------------------------------------------------%

include "globals.mzn";

array[1..2] of var opt 1.0..2.0: x::add_to_output;


constraint sum(x)=1.0; 

% This fails:
%constraint x[1]+x[2]=1.0; 

%-----------------------------------------------------------------------------%
solve satisfy;

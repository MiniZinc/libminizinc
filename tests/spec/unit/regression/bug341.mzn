/***
!Test
expected:
- !Result
  solution: !Solution
    S:
    - !Range 1..3
    - !Range 4..6
- !Result
  solution: !Solution
    S:
    - !!set {}
    - !!set {}
***/

% Regression test for bug #341: mzn2fzn 1.5.0 was incorrectly tightening the
% bounds on the array S to 4..3.  The problem was that the bounds inferred for
% each element of the array (which may differ) were being applied to the array
% as a whole.

array[1..2] of var set of 1..6: S; 
                                                                                                                                                                          
constraint 
  S[1] subset {1,2,3} /\ 
  S[2] subset {4,5,6} 
; 
                                                                                                                                                                          
solve satisfy; 
                                                                                                                                                                          
output [ 
    "S = array1d(1..2, ", show(S), ";" 
]; 

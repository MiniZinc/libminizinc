/***
!Test
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    x: 
    - !!set {0}
    - !Range 0..9
- !Result
  status: SATISFIED
  solution: !Solution
    x: 
    - !!set {0}
    - !!set {}
***/

array [1..2] of var set of 0..9: x :: add_to_output = [{0}, _];
solve satisfy;

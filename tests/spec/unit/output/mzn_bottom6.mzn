/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: '[1, _, _, _, _, _, _, _]'
***/

solve satisfy;
output [show(array3d(1..2,1..2,1..2,[1,_,_,_,_,_,_,_]))];

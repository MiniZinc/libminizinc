/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    i: 1
***/

var 1..1: i;
predicate losseq(array[int] of var int: a) = sum([1 | e in a where e >= 1]) = 1;
constraint losseq([0,0,0,1]);
solve satisfy;
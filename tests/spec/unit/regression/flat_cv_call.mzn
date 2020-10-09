/***
!Test
expected:
- !Result
  solution: !Solution
    objective: 4
    a: [0,0,2,2]
    rem: 2
***/

function int: forceN(array[int] of var int: x, int: n) =
  let {
    constraint forall(i in 1..n) ( x[i] = 0 );
  } in length(x) - n;

array[1..4] of var 0..2: a;
var int: rem;
constraint rem = forceN(a, 2);

solve maximize sum(a);

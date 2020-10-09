/***
!Test
expected:
- !Result
  solution: !Solution
    objective: 4
    a: [0,0,2,2]
    num: 4
***/

array[1..4] of var 0..2: a ::add_to_output;
int: num ::add_to_output = (let {
    constraint forall(i in 1..2) ( a[i] = 0 );
  } in length(a) - 2) + 2;

solve maximize sum(a);

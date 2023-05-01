/***
!Test
solvers: [gecode]
expected: 
  - !Result
    solution: !Solution
      vars: [2, 1]
      c: 1
  - !Result
    solution: !Solution
      vars: [1, 2]
      c: 1
***/
array[1..2] of opt 1..1: values = [1, <>];
array[1..2] of var 1..2: vars;
function var 1..2: identity(var 1..2: k) = (k - 1) mod 2 + 1;
var 1..2: c ::output = count(x in 1..2)(values[identity(vars[x])] = 1) ::mzn_break_here;
constraint c = 1;

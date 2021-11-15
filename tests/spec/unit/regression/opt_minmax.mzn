/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution
    xmax: 10
    xmin: 1
    ymax: 10.0
    ymin: 1.0
***/
array[_] of var opt 1..10: x = [<>, 10, _];
constraint sum(x) = 11;
any: xmax ::output = max(x);
any: xmin ::output = min(x);
array[_] of var opt 1.0..10.0: y = [<>, 10.0, _];
constraint sum(y) = 11.0;
any: ymax ::output = max(y);
any: ymin ::output = min(y);

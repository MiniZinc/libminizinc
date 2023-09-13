/***
!Test
solvers: [gecode]
expected:
  - !Result
    solution: !Solution
      wanted: !!set {1}
  - !Result
    solution: !Solution
      wanted: !!set {4}
***/

enum color = {red, green, blue};
set of int: shirt_ids = 1..5;
array[shirt_ids] of color: shirts = [red, green, blue, red, green];
var set of shirt_ids: wanted;
constraint forall (c in color)(count(i in wanted)(shirts[i] = c) = (c = red));
solve satisfy;

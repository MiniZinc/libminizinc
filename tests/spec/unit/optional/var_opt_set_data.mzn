/***
!Test
solvers: [gecode, chuffed]
extra_files: [var_opt_set_data.dzn]
expected: !Result
  solution: !Solution
    x: null
    y: !!set {1, 3}
    t: [!!set {2, 3}, 1]
    r:
      f: null
***/

var opt set of 1..3: x;
var opt set of 1..3: y;
tuple(var opt set of 1..3, int): t;
record(var opt set of 1..3: f): r;
constraint occurs(t.1) /\ absent(r.f);

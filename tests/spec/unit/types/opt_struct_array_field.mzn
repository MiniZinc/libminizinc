/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    present: {"a": [1, 2]}
    missing: null
    vp: [1, 2]
    vm: []
    tp: [7, 8]
***/

% An optional struct may have array-typed fields. The array cannot be projected out *through* the
% optionality (`present.a' is a type error: there is no absent array to stand for it when the
% record is absent -- see opt_struct_field_access_array.mzn), but `deopt' strips the optionality
% first, after which the field is an ordinary access.
opt record(array [1..2] of int: a): present :: output = (a: [1, 2]);
opt record(array [1..2] of int: a): missing :: output = <>;

array [int] of int: vp :: output = if occurs(present) then deopt(present).a else [] endif;
array [int] of int: vm :: output = if occurs(missing) then deopt(missing).a else [] endif;

% Same for a tuple with an array element.
opt tuple(int, array [1..2] of int): t = (1, [7, 8]);
array [int] of int: tp :: output = if occurs(t) then deopt(t).2 else [] endif;

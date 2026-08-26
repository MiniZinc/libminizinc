/***
!Test
type: compile
solvers: [gecode]
extra_files: [checker_array_annotations.mzc.mzn]
expected: !FlatZinc checker_array_annotations.fzn
***/

% A checker-required, enum-indexed array becomes a 1-indexed FlatZinc array.
% Its checker annotations must not be emitted in the FlatZinc model.
enum E = {A, B};
array [E] of var 0..1: x;

solve satisfy;

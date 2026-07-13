/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: [1, 2]
    b: null
    r: {"x": 1, "y": true}
    s: null
    arr: [[1, 2], null, [3, 4]]
***/

% `opt t' for a struct means "the whole struct, or absent". The fields keep their declared
% types; it is not a distribution of `opt' into the fields.
opt tuple(int, int): a :: output = (1, 2);
opt tuple(int, int): b :: output = <>;
opt record(int: x, bool: y): r :: output = (x: 1, y: true);
opt record(int: x, bool: y): s :: output = <>;

% Arrays of optional structs, mixing present and absent elements.
array [1..3] of opt tuple(int, int): arr :: output = [(1, 2), <>, (3, 4)];

/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: |-
      (1, 2)|<>|(x: 1, y: true)|<>|[(1, 2), <>]
      (1, 2)|<>|(x: 1, y: true)|<>|[(1, 2), <>]
***/

% `show' of an optional record keeps the field names, and string interpolation of an optional
% struct works in an output item (it goes through `format', which is not a builtin and so has to
% be found again when the output model is built).
opt tuple(int, int): a = (1, 2);
opt tuple(int, int): b = <>;
opt record(int: x, bool: y): r = (x: 1, y: true);
opt record(int: x, bool: y): s = <>;
array [1..2] of opt tuple(int, int): arr = [(1, 2), <>];

output [
  show(a), "|", show(b), "|", show(r), "|", show(s), "|", show(arr), "\n",
  % Two interpolations of the same specialised instance in one output item: the second call has
  % to match the function copy created for the first.
  "\(a)|\(b)|\(r)|\(s)|\(arr)",
];

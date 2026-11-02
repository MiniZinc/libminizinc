/***
!Test
extra_files:
- json_array_empty_dimension.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: !Trim |
      1..3 {}
      1..2 {} 1..2
      1..2 {} 1..2
      1..3 {}
***/

% JSON arrays for declarations with an empty index set. Shifting the index sets of the data to
% the declared ones failed on the empty set (minimum of empty set), and an empty array may have
% fewer dimensions in JSON than declared.
int: n;
set of int: G = 1..n;
set of int: P = 1..0;
enum Q = {};
array[G, P] of bool: a;
array[1..2, P, 1..2] of int: b;
array[1..2, P, 1..2] of int: c;
array[1..3, Q] of int: d;
output [
  show(index_set_1of2(a)), " ", show(index_set_2of2(a)), "\n",
  show(index_set_1of3(b)), " ", show(index_set_2of3(b)), " ", show(index_set_3of3(b)), "\n",
  show(index_set_1of3(c)), " ", show(index_set_2of3(c)), " ", show(index_set_3of3(c)), "\n",
  show(index_set_1of2(d)), " ", show(index_set_2of2(d)), "\n",
];

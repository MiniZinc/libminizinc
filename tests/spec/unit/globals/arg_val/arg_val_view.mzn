/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/
% A slice of an array equals a plain array with the same index set and elements, so a lookup in
% the slice reuses the map built for the plain array (and vice versa). The data dependencies make
% sure the counter is read before and after the lookups.
include "arg_val.mzn";

array [1..2, 1..3] of string: m = [| "a", "b", "c" | "d", "e", "f" |];
array [1..3] of string: r2 = ["d", "e", "f"];
array [1..3] of int: xs = [4, 5, 6];
array [1..2, 1..3] of int: g = [| 1, 2, 3 | 4, 5, 6 |];

int: b0 = mzn_arg_val_index_builds();
int: plain = if b0 >= 0 then arg_val(r2, "e") + arg_val(xs, 6) else 0 endif;
int: sliced = if plain > 0 then arg_val(m[2, ..], "e") + arg_val(g[2, ..], 6) else 0 endif;
int: b1 = if sliced > 0 then mzn_arg_val_index_builds() else -1 endif;

constraint assert(plain = 5 /\ sliced = 5, "wrong lookup result");
constraint assert(b1 - b0 = 2, "expected two map builds, got \(b1 - b0)");

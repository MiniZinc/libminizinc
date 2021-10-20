/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc annotate_from_array.fzn
***/

ann: one;
ann: two;
array[int] of ann: anns = [one, two];
array[int] of var int: bs ::output = [ let {
  var 1..2: b::anns[i];
} in b | i in index_set(anns) ];

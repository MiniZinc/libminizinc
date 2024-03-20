/***
!Test
expected:
- !Result
  status: SATISFIED
solvers: [gecode]
***/
type foo = record(int: a);
type bar = record(string: b);

array[int] of bar: bars = [];
array[int] of (foo ++ bar): baz = [ (a: i) ++ bars[i] | i in index_set(bars) ];
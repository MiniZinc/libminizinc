% This file contains redefinitions of standard builtins for version 2.3.3
% that can be overridden by solvers.

predicate float_set_in(var float: x, set of float: S) =
  let {
    array[int] of float: r1d = set_to_ranges(S);
    array[int,1..2] of float: r = array2d(1..length(r1d) div 2,1..2,r1d)
  } in exists (i in index_set_1of2(r)) (
    if r[i,1]=r[i,2] then x=r[i,1] else (x>=r[i,1] /\ x<=r[i,2]) endif
  );

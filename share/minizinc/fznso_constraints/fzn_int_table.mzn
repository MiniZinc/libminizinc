% The constraint `int_table`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_table` and the body below
% is dropped in favour of the propagator, or ship a file of this name with a
% decomposition of your own.

% A FlatZinc array is one-dimensional and one-based, so the tuples are passed
% flat, laid out one tuple after another, and both arrays are declared `list of`
% — which is what says one-based, and what the type-checker holds the caller to.
% The two-dimensional form the standard library's `table` calls forwards to it.

predicate fzn_int_table(list of var int: xs, list of int: tuples) =
  let {
    int: n = length(xs);
  } in if n = 0 then
    true
  else
    let {
      int: m = length(tuples) div n;
      var 1..m: row;
      array [1..n, 1..m] of int: by_position =
        array2d(1..n, 1..m, [tuples[(k - 1) * n + j] | j in 1..n, k in 1..m]);
    } in forall (j in 1..n) (by_position[j, row] = xs[j])
  endif;

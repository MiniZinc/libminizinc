% The constraint `bool_table`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_table` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% See `fzn_table_int.mzn`: the tuples are passed as one flat, one-based array.

predicate fzn_bool_table(list of var bool: xs, list of bool: tuples) =
  let {
    int: n = length(xs);
  } in if n = 0 then
    true
  else
    let {
      int: m = length(tuples) div n;
      var 1..m: row;
      array [1..n, 1..m] of bool: by_position =
        array2d(1..n, 1..m, [tuples[(k - 1) * n + j] | j in 1..n, k in 1..m]);
    } in forall (j in 1..n) (by_position[j, row] = xs[j])
  endif;

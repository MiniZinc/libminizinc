% The constraint `int_table_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_table_reif` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% See `fzn_table_int.mzn`: the tuples are passed as one flat, one-based array.

predicate fzn_int_table_reif(list of var int: xs, list of int: tuples, var bool: b) =
  let {
    int: n = length(xs);
  } in if n = 0 then
    b
  else
    b = exists (k in 1..length(tuples) div n) (
      forall (j in 1..n) (xs[j] = tuples[(k - 1) * n + j])
    )
  endif;

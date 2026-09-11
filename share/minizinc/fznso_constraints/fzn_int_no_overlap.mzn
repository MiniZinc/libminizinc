% The constraint `int_no_overlap`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_no_overlap` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% `diffn` is called `no_overlap` everywhere outside the FlatZinc builtin list,
% and pairing each coordinate with its own extent reads better than two
% coordinate arrays followed by two extent arrays. The solver-facing form takes
% `list of`, which is what states that the arrays are one-based.

predicate fzn_int_no_overlap(
  list of var int: x,
  list of var int: dx,
  list of var int: y,
  list of var int: dy,
) =
  forall (i, j in index_set(x) where i < j) (
    x[i] + dx[i] <= x[j] \/ y[i] + dy[i] <= y[j] \/ x[j] + dx[j] <= x[i] \/ y[j] + dy[j] <= y[i]
  );

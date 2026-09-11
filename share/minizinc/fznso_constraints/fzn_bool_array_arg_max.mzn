% The constraint `bool_array_arg_max`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_array_arg_max` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% A FlatZinc array is one-based, so the index the caller numbers `xs` from is
% passed explicitly rather than left implicit in an `index_set` the solver never
% receives. `list of` is what states the one-basing, and what the type-checker
% holds the caller to.

predicate fzn_bool_array_arg_max(list of var bool: xs, int: offset, var int: index) =
  let {
    int: n = length(xs);
    array [1..n] of var int: w = array1d(1..n, [n * xs[j] + (n - j) | j in 1..n]);
    var int: mx = max(w);
  } in forall (j in 1..n) ((index != offset + j - 1) = (mx > w[j]));

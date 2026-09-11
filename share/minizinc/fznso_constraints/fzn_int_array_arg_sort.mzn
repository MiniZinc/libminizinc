% The constraint `int_array_arg_sort`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_array_arg_sort` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% A FlatZinc array is one-based, so the index the caller numbers `xs` from is
% passed explicitly rather than left implicit in an `index_set` the solver never
% receives. `list of` is what states the one-basing, and what the type-checker
% holds the caller to.

include "all_different.mzn";

predicate fzn_int_array_arg_sort(
  list of var int: xs,
  int: offset,
  list of var int: permutation,
) =
  let {
    array [offset..offset + length(xs) - 1] of var int: x =
      array1d(offset..offset + length(xs) - 1, xs);
  } in (
    all_different(permutation) /\
      forall (j in 1..length(x) - 1) (
        x[permutation[j]] <= x[permutation[j + 1]] /\ (x[permutation[j]] == x[permutation[j + 1]] -> permutation[j] < permutation[j + 1])
      )
  );

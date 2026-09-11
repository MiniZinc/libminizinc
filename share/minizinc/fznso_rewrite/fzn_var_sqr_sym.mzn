% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The eight symmetries of a square board, expressed as permutations of the flat
% matrix. A FlatZinc array is one-dimensional, so the side of the square travels
% alongside it rather than being read off an `index_set` the solver never
% receives.

include "fzn_int_lex_least_under_transposition.mzn";

predicate fzn_var_sqr_sym(array [int, int] of var int: xs) =
  fzn_int_lex_least_under_transposition(array1d(xs), card(index_set_2of2(xs)));

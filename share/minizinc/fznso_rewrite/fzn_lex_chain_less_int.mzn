% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_lex_chain_less_int`; the registry names it for the type
% it takes and puts that first.
%
% A FlatZinc array is one-dimensional, so the matrix is passed flat, one row
% after another, with the number of columns alongside — `std` reads that off an
% `index_set` the solver never receives. The order is on the *columns*.

include "fzn_int_lex_chain_less.mzn";

predicate fzn_lex_chain_less_int(array [int, int] of var int: xs) =
  fzn_int_lex_chain_less(array1d(xs), card(index_set_2of2(xs)));

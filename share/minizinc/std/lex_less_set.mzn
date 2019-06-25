include "lex_less.mzn";
include "fzn_lex_less_set.mzn";
include "fzn_lex_less_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_less_set(array[int] of var set of int: x,
                       array[int] of var set of int: y) =
    fzn_lex_less_set(x, y);

predicate lex_lt_set(array[int] of var set of int: x,
                     array[int] of var set of int: y) =
    lex_less(x, y);

%-----------------------------------------------------------------------------%

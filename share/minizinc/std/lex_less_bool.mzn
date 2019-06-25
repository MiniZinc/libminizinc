include "lex_less.mzn";
include "fzn_lex_less_bool.mzn";
include "fzn_lex_less_bool_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_less_bool(array[int] of var bool: x,
                        array[int] of var bool: y) =
    fzn_lex_less_bool(x, y);

predicate lex_lt_bool(array[int] of var bool: x,
                      array[int] of var bool: y) =
    lex_less(x, y);

%-----------------------------------------------------------------------------%

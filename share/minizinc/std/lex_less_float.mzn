include "lex_less.mzn";
include "fzn_lex_less_float.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_less_float(array[int] of var float: x,
                         array[int] of var float: y) =
    fzn_lex_less_float(x, y);

predicate lex_less_float_reif(array[int] of var float: x,
                              array[int] of var float: y,
                              var bool: c) =
    fzn_lex_less_float_reif(x, y, c);

predicate lex_lt_float(array[int] of var float: x,
                       array[int] of var float: y) =
    lex_less(x, y);

%-----------------------------------------------------------------------------%

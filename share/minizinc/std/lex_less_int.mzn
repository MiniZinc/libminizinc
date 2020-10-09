include "lex_less.mzn";
include "fzn_lex_less_int.mzn";
include "fzn_lex_less_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_less_int(array[int] of var int: x,
                       array[int] of var int: y) =
    fzn_lex_less_int(x, y);

predicate lex_lt_int(array[int] of var int: x,
                     array[int] of var int: y) =
          lex_less(x, y);

%-----------------------------------------------------------------------------%

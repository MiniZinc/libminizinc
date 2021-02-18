include "lex_less.mzn";
include "fzn_lex_less_float.mzn";
include "fzn_lex_less_float_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_less_float(array[int] of var float: x ::promise_ctx_antitone,
                         array[int] of var float: y ::promise_ctx_monotone) =
    fzn_lex_less_float(x, y);

predicate lex_lt_float(array[int] of var float: x ::promise_ctx_antitone,
                       array[int] of var float: y ::promise_ctx_monotone) =
    lex_less(x, y);

%-----------------------------------------------------------------------------%

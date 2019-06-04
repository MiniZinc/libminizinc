include "fzn_lex_lesseq_float.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_lesseq_float(array[int] of var float: x,
                           array[int] of var float: y) =
    fzn_lex_lesseq_float(x, y);

predicate lex_lesseq_float_reif(array[int] of var float: x,
                                array[int] of var float: y,
                                var bool: c) =
    fzn_lex_lesseq_float_reif(x, y, c);

predicate lex_leq_float(array[int] of var float: x,
                        array[int] of var float: y) =
    lex_lesseq_float(x, y);

%-----------------------------------------------------------------------------%

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

include "fzn_lex_less_int.mzn";

predicate fzn_lex_less_bool(array [int] of var bool: x, array [int] of var bool: y) =
  fzn_lex_less_int(x, y);

%-----------------------------------------------------------------------------%

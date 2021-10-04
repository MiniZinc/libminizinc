include "fzn_member_set.mzn";
include "fzn_member_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array or set 'x'.
%-----------------------------------------------------------------------------%

predicate member_set(array[int] of var set of $$E: x, var set of $$E: y) =
    fzn_member_set(x, y);

include "fzn_member_float.mzn";
include "fzn_member_float_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that 'y' occurs in the array or set 'x'.
%-----------------------------------------------------------------------------%

predicate member_float(array [int] of var float: x, var float: y) = fzn_member_float(x, y);

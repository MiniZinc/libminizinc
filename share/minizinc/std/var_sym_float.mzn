%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for variable symmetries.
% Prunes any symmetrically equivalent solutions by taking an order for the
% variables then constraining their values to follow the same order.
%-----------------------------------------------------------------------------%

include "increasing.mzn";

predicate var_sym_float(array[int] of var float: x) :: symmetry =
    increasing(x);

predicate var_sym_float_ord(array[int] of var float: x,
                            array[int] of var float: order) =
    let { array[1..length(x)] of var float: x2 = sort_by_var(x, order) }
    in
    var_sym_float(x2);

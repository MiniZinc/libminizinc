%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for variable symmetries.
% Prunes any symmetrically equivalent solutions by taking an order for the
% variables then constraining their values to follow the same order.
%-----------------------------------------------------------------------------%

include "increasing.mzn";

predicate var_sym_int(array[int] of var int: x) :: symmetry =
    increasing(x);

predicate var_sym_int_ord(array[int] of var int: x,
                          array[int] of var int: order) =
    let { array[1..length(x)] of var int: x2 = sort_by_var(x, order) }
    in
    var_sym_int(x2);

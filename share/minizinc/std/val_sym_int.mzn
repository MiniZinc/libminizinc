%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for value symmetries.
% Prunes any symmetrically equivalent solutions by taking an order for the
% variables then constraining their values so that the first occurences of each
% value across the variables is ordered.
%-----------------------------------------------------------------------------%

include "value_precede_chain.mzn";

predicate val_sym_int(array[int] of var int: x,
                      array[int] of int: s) :: symmetry =
    value_precede_chain(s, x);

predicate val_sym_int_ord(array[int] of var int: x,
                          array[int] of int: s,
                          array[int] of var int: order) =
    let { array[1..length(x)] of var int: x2 = sort_by_var(x, order),
          array[1..length(s)] of int: s2 = sort(s) }
    in
    val_sym_int(x2, s2);

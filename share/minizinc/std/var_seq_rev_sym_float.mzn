%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for variable sequence reverse
% symmetries.
% Prunes any equivalent solutions using a variable permutation symmetry with
% the permutation that reverses the sequence.
%-----------------------------------------------------------------------------%

include "var_perm_sym.mzn";

predicate var_seq_rev_sym_float(array[int] of var float: x) =
    let { int: l = length(x),
          array[1..2,1..l] of 1..l: y = array2d(1..2, 1..l,
              [ if i == 1 then j else l - j + 1 endif
                  | i in 1..2, j in 1..l ] ) }
    in var_perm_sym(x,y);

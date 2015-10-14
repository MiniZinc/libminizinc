%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for variable rectangle flip
% symmetries.
% Prunes any symmetrically equivalent solutions using a variable permutation
% symmetry for flips on the two lines.
%-----------------------------------------------------------------------------%

include "var_perm_sym.mzn";

predicate var_rect_flip_sym_float(array[int,int] of var float: x) =
    let { int: nr = card(index_set_1of2(x));
          int: nc = card(index_set_2of2(x));
          int: l = nr * nc;
          array[1..l] of var float: y = [ x[i,j] | i in index_set_1of2(x),
                                                 j in index_set_2of2(x) ];
          array[1..3,1..l] of 1..l: p = array2d(1..3,1..l,
              [ if     k == 1 then (i - 1)*nc + j
                elseif k == 2 then (nr - i)*nc + j            % x flip
                else               i*nc - j + 1               % y flip
                endif
              | k in 1..3, i in 1..nr, j in 1..nc]) }
    in
    var_perm_sym(y,p);

%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for variable rectangle flip
% symmetries.
% Prunes any symmetrically equivalent solutions using a variable permutation
% symmetry for flips on the four lines.
%-----------------------------------------------------------------------------%

include "var_perm_sym.mzn";

predicate var_sqr_flip_sym_int(array[int,int] of var int: x) =
    let { int: n = card(index_set_1of2(x));
          int: n2 = card(index_set_2of2(x));
          constraint assert(n = n2,
                            "square symmetry applied to non-square array");
          int: l = n * n;
          array[1..l] of var int: y = [ x[i,j] | i in index_set_1of2(x),
                                                 j in index_set_2of2(x) ];
          array[1..5,1..l] of 1..l: p = array2d(1..5,1..l,
              [ if k == 1 then i*n + j - n
                elseif k == 2 then (n - i)*n + j           % x flip
                elseif k == 3 then (i*n - n) + (n - j + 1) % y flip
                elseif k == 4 then (n - j)*n + (n - i + 1) % d2 flip
                else               (j*n - n) + i           % d1 flip
                endif
              | k in 1..5, i,j in 1..n]) }
    in
    var_perm_sym(y,p);

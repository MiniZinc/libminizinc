%-----------------------------------------------------------------------------%
% Default static symmetry breaking constraints for variable square rotation
% symmetries.
% Prunes any symmetrically equivalent solutions using a variable permutation
% symmetry with a permutation for each of the symmetries in the size four
% square rotation symmetry group.
%-----------------------------------------------------------------------------%

include "var_perm_sym.mzn";

predicate var_sqr_rot_sym_int(array[int,int] of var int: x) =
    let { int: n = card(index_set_1of2(x));
          int: n2 = card(index_set_2of2(x));
          constraint assert(n = n2,
                            "square symmetry applied to non-square array");
          int: l = n * n;
          array[1..l] of var int: y = [ x[i,j] | i in index_set_1of2(x),
                                                 j in index_set_2of2(x) ];
          array[1..4,1..l] of 1..l: p = array2d(1..4,1..l,
              [ if     k == 1 then i*n + j - n
                elseif k == 2 then (n - j)*n + i           % r270
                elseif k == 3 then (n - i)*n + (n - j)+1   % r180
                else               (j*n - n) + (n - i + 1) % r90
                endif
              | k in 1..4, i,j in 1..n]) }
  in
  var_perm_sym(y,p);

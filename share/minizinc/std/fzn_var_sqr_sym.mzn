include "fzn_var_perm_sym.mzn";

predicate fzn_var_sqr_sym(array[$$X, $$X] of var $$Y: x) =
  let {
    int: n = card(index_set_1of2(x));
    int: l = n * n;
    array[1..l] of var int: y = [ x[i,j] | i in index_set_1of2(x), j in index_set_2of2(x) ];
    array[1..8, 1..l] of 1..l: p = array2d(1..8, 1..l,
      [ if k == 1 then i*n + j - n 
        elseif k == 2 then (n - j)*n + i           % r270
        elseif k == 3 then (n - i)*n + (n - j)+1   % r180
        elseif k == 4 then (j*n - n) + (n - i + 1) % r90
        elseif k == 5 then (n - i)*n + j           % x flip
        elseif k == 6 then (i*n - n) + (n - j + 1) % y flip
        elseif k == 7 then (n - j)*n + (n - i + 1) % d2 flip
        else               (j*n - n) + i           % d1 flip
        endif
      | k in 1..8, i, j in 1..n]
    );
  } in fzn_var_perm_sym(y, p);

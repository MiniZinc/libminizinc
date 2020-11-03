predicate fzn_table_int_opt(array[int] of var opt int: x, array[int, int] of opt int: t) =
  let {
    int: l = min(index_set(x)),
    int: u = max(index_set(x)),
    int: lt = min(index_set_1of2(t)),
    int: ut = max(index_set_1of2(t)),
    var lt..ut: i,
    array[l..u, lt..ut] of opt int: t_transposed =
        array2d(l..u, lt..ut, [ t[k,j] | j in l..u, k in lt..ut ])
  } in forall(j in l..u) (
    % Having the variable index component at the left position
    % means that the nD-to-1D array translation during Mzn-to-Fzn
    % will generate at most an offset constraint, instead of a
    % scaling + offset constraint.
    t_transposed[j,i] ~= x[j]
  );

predicate fzn_table_int_opt(array [int] of var opt int: xs, array [int, int] of opt int: tuples) =
  let {
    int: l = min(index_set(xs));
    int: u = max(index_set(xs));
    int: lt = min(index_set_1of2(tuples));
    int: ut = max(index_set_1of2(tuples));
    var lt..ut: i;
    array [l..u, lt..ut] of opt int: t_transposed =
      array2d(l..u, lt..ut, [tuples[k, j] | j in l..u, k in lt..ut]);
  } in forall (j in l..u) (
    % Having the variable index component at the left position
    % means that the nD-to-1D array translation during Mzn-to-Fzn
    % will generate at most an offset constraint, instead of a
    % scaling + offset constraint.
    t_transposed[j, i] ~= xs[j]
  );

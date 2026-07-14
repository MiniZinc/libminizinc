%-----------------------------------------------------------------------------%
% A table constraint: table(xs, tuples) represents the constraint xs in tuples where we
% consider each row in tuples to be a tuple and tuples as a set of tuples.
%-----------------------------------------------------------------------------%

predicate fzn_table_bool(array [int] of var bool: xs, array [int, int] of bool: tuples) =
  let {
    int: l = min(index_set(xs));
    int: u = max(index_set(xs));
    int: lt = min(index_set_1of2(tuples));
    int: ut = max(index_set_1of2(tuples));
    var lt..ut: i;
    array [l..u, lt..ut] of bool: t_transposed =
      array2d(l..u, lt..ut, [tuples[k, j] | j in l..u, k in lt..ut]);
  } in forall (j in l..u) (
    % Having the variable index component at the left position
    % means that the nD-to-1D array translation during Mzn-to-Fzn
    % will generate at most an offset constraint, instead of a
    % scaling + offset constraint.
    %
    t_transposed[j, i] = xs[j]
    %
    % tuples[i,j] = xs[j]
  );

predicate geas_table_int(array [int] of var int: x, array [int] of int: t);

predicate fzn_table_int(array [int] of var int: x, array [int, int] of int: t) =
  assert (index_set_2of2(t) == index_set(x),
    "The second dimension of the table must equal the number of variables "
      ++ "in the first argument",
    geas_table_int(x, [ t[i,j] | i in index_set_1of2(t), j in index_set_2of2(t) ])
  );

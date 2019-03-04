predicate geas_table_int(array [int] of var int: x, array [int] of int: t);

predicate table_int(array [int] of var int: x, array [int, int] of int: t) =
  %% TODO: Check index sets
  geas_table_int(x, [ t[i,j] | i in index_set_1of2(t), j in index_set_2of2(t) ]);

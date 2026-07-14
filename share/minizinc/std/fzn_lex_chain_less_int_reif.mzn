include "lex_less.mzn";

predicate fzn_lex_chain_less_int_reif(array [int, int] of var int: xs, var bool: b) =
  let {
    set of int: is2 = index_set_2of2(xs);
  } in b <-> (forall (j in is2 where j + 1 in is2) (lex_less(col(xs, j), col(xs, j + 1))));

include "lex_lesseq.mzn";

predicate fzn_lex_chain_lesseq_orbitope_reif(
  array [int, int] of var int: xs,
  int: _kind,
  var bool: b,
) =
  let {
    set of int: is2 = index_set_2of2(xs);
  } in b <->
    (
      forall (j in is2 where j + 1 in is2) (lex_lesseq(col(xs, j), col(xs, j + 1))) /\
        if 1 == _kind then
          forall (i in index_set_1of2(xs)) (1 == sum(row(xs, i)))
        elseif 2 == _kind then
          forall (i in index_set_1of2(xs)) (1 >= sum(row(xs, i)))
        else
          true
        endif
    );

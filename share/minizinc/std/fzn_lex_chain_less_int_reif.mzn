include "lex_less.mzn";

predicate fzn_lex_chain_less_int_reif(
        array[int, int] of var int: a, var bool: b) =
    let {
        set of int: is2 = index_set_2of2(a);
    } in b <-> (
        forall(j in is2 where j+1 in is2) (
            lex_less(col(a, j), col(a, j+1))
        )
    );

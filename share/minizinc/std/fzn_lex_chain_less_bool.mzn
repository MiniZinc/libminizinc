include "lex_less.mzn";

predicate fzn_lex_chain_less_bool(array[int, int] of var bool: a) =
    let {
        set of int: is2 = index_set_2of2(a);
    } in (
        forall(j in is2 where j+1 in is2) (
            lex_less(col(a, j), col(a, j+1))
        )
    );

include "lex_less.mzn";

predicate fzn_lex_chain_less_bool(array[int, int] of var bool: a) =
    let {
        int: lb2 = min(index_set_2of2(a)),
        int: ub2 = max(index_set_2of2(a))
    } in (
        forall(j in lb2 + 1 .. ub2) (
            lex_less(col(a, j-1), col(a, j))
        )
    );

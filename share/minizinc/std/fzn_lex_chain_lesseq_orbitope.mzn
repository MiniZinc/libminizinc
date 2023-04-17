include "lex_lesseq.mzn";

predicate fzn_lex_chain_lesseq_orbitope(
        array[int, int] of var int: a, int: kind) =
    let {
        set of int: is2 = index_set_2of2(a);
    } in (
        forall(j in is2 where j+1 in is2) (
            lex_lesseq(col(a, j), col(a, j+1))
        )
        /\
        if 1==kind then
            forall(i in index_set_1of2(a))(
                1 == sum(row(a, i))
            )
        elseif 2==kind then
            forall(i in index_set_1of2(a))(
                1 >= sum(row(a, i))
            )
        else true
        endif
    );

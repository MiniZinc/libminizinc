include "lex_chain_lesseq.mzn";

predicate fzn_lex2(array[int, int] of var int: x) =
    let {
        int: lbx1 = min(index_set_1of2(x)),
        int: ubx1 = max(index_set_1of2(x)),
        int: lbx2 = min(index_set_2of2(x)),
        int: ubx2 = max(index_set_2of2(x))
    } in (
        lex_chain_lesseq(x)
        /\
        lex_chain_lesseq(                   %% transpose
            array2d(index_set_2of2(x), index_set_1of2(x),
                [x[j, i] | i in index_set_1of2(x), j in index_set_2of2(x)]
            )
        )
    );

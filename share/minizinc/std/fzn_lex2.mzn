include "lex_chain_lesseq.mzn";

predicate fzn_lex2(array[int, int] of var int: x) =
    lex_chain_lesseq(x)
    /\
    lex_chain_lesseq(                   %% transpose
        array2d(index_set_2of2(x), index_set_1of2(x),
            [x[i, j] | j in index_set_2of2(x), i in index_set_1of2(x)]
        )
    );

include "lex_chain_less.mzn";

predicate fzn_strict_lex2_reif(array[int, int] of var int: x, var bool: b) =
    b <-> (
        lex_chain_less(x)
        /\
        lex_chain_less(                                  %% transpose
            array2d(index_set_2of2(x), index_set_1of2(x),
                [x[i, j] | j in index_set_2of2(x), i in index_set_1of2(x)]
            )
        )
    );

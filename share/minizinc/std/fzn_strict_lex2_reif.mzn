include "lex_less.mzn";

predicate fzn_strict_lex2_reif(array[int, int] of var int: x, var bool: b) =
    let {
        int: lbx1 = min(index_set_1of2(x)),
        int: ubx1 = max(index_set_1of2(x)),
        int: lbx2 = min(index_set_2of2(x)),
        int: ubx2 = max(index_set_2of2(x))
    } in b <-> (
        forall(i in lbx1 + 1 .. ubx1) (
            lex_less([x[i - 1, j] | j in index_set_2of2(x)],
                     [x[i,     j] | j in index_set_2of2(x)]
            )
        )
        /\

        forall(j in lbx2 + 1 .. ubx2) (
            lex_less([x[i, j - 1] | i in index_set_1of2(x)],
                     [x[i, j    ] | i in index_set_1of2(x)]
            )
        )
    );

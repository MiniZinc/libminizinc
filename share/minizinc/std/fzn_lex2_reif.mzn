include "lex_lesseq.mzn";

predicate fzn_lex2_reif(array [int, int] of var int: xs, var bool: b) =
  b <->
    (
      lex_chain_lesseq(xs) /\
        lex_chain_lesseq( %% transpose
          array2d(
            index_set_2of2(xs),
            index_set_1of2(xs),
            [xs[i, j] | j in index_set_2of2(xs), i in index_set_1of2(xs)],
          ),
        )
    );

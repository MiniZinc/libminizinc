include "lex_chain_less.mzn";

/** @group globals.lexicographic
  Requires that the columns of matrix \a xs are lexicographically sorted,
  strictly decreasing.
*/
predicate lex_chain_greater(array [$$E, $$F] of var bool: xs) =
  if card(index_set_2of2(xs)) > 1 then
    let {
      any: aa = index2int(xs);
    } in lex_chain_less(
      arrayXd(
        aa,
        [
          aa[i, max(index_set_2of2(aa)) - j + min(index_set_2of2(aa))] |
            i in index_set_1of2(aa),
            j in index_set_2of2(aa),
        ],
      ),
    )
  endif;

/** @group globals.lexicographic
  Requires that the columns of matrix \a xs are lexicographically sorted,
  strictly decreasing.
*/
predicate lex_chain_greater(array [$$E, $$F] of var $$T: xs) =
  if card(index_set_2of2(xs)) > 1 then
    let {
      any: aa = index2int(enum2int(xs));
    } in lex_chain_less(
      arrayXd(
        aa,
        [
          aa[i, max(index_set_2of2(aa)) - j + min(index_set_2of2(aa))] |
            i in index_set_1of2(aa),
            j in index_set_2of2(aa),
        ],
      ),
    )
  endif;

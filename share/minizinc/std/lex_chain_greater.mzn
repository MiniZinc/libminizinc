include "lex_chain_less.mzn";

/** @group globals.lexicographic
  Requires that the columns of matrix \a a are lexicographically sorted,
  strictly decreasing.
*/
predicate lex_chain_greater(array [$$E, $$F] of var bool: a) =
  if card(index_set_2of2(a)) > 1 then
    let {
      any: aa = index2int(a);
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
  Requires that the columns of matrix \a a are lexicographically sorted,
  strictly decreasing.
*/
predicate lex_chain_greater(array [$$E, $$F] of var $$T: a) =
  if card(index_set_2of2(a)) > 1 then
    let {
      any: aa = index2int(enum2int(a));
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

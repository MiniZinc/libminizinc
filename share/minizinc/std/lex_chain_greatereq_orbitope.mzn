include "lex_chain_lesseq_orbitope.mzn";

/** @group globals.lexicographic
  Requires that the columns of binary matrix \a a are
  lexicographically sorted, non-increasing.
  Moreover, the second parameter \a kind has the following meaning:
  0: no further constraints, 1: set-partitioning orbitope,
  2: set-packing orbitope
*/
predicate lex_chain_greatereq_orbitope(array [$$E, $$F] of var int: a, int: kind) =
  if card(index_set_2of2(a)) > 1 then
    let {
      any: aa = index2int(a);
    } in lex_chain_lesseq_orbitope(
      array2d(
        index_set_1of2(aa),
        index_set_2of2(aa),
        [
          aa[i, max(index_set_2of2(aa)) - j + min(index_set_2of2(aa))] |
            i in index_set_1of2(aa),
            j in index_set_2of2(aa),
        ],
      ),
      kind,
    )
  endif;

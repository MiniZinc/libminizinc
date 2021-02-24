include "fzn_lex_chain_lesseq_orbitope.mzn";
include "fzn_lex_chain_lesseq_orbitope_reif.mzn";

/** @group globals.lexicographic
  Requires that the columns of binary matrix \a a are
  lexicographically sorted, non-decreasing.
  Moreover, the second parameter has the following meaning:
  0: no further constraints, 1: set-partitioning orbitope,
  2: set-packing orbitope
*/
predicate lex_chain_lesseq_orbitope(
        array[int, int] of var int: a, int: kind) = 
    assert(dom_array(a) subset 0..1,
        "lex_chain_lesseq_orbitope: matrix \(a) is non-binary")
    /\
    if 1>=card(index_set_2of2(a)) then true
    else fzn_lex_chain_lesseq_orbitope(a, kind) endif;


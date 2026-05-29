include "fzn_lex_chain_lesseq_orbitope.mzn";
include "fzn_lex_chain_lesseq_orbitope_reif.mzn";

/** @group globals.lexicographic
  Requires that the columns of binary matrix \a xs are
  lexicographically sorted, non-decreasing.
  Moreover, the second parameter \a _kind has the following meaning:
  0: no further constraints, 1: set-partitioning orbitope,
  2: set-packing orbitope
*/
predicate lex_chain_lesseq_orbitope(array [$$E, $$F] of var int: xs, int: _kind) =
  assert(dom_array(xs) subset 0..1, "lex_chain_lesseq_orbitope: matrix \(xs) is non-binary") /\
    if card(index_set_2of2(xs)) > 1 then fzn_lex_chain_lesseq_orbitope(index2int(xs), _kind) endif;

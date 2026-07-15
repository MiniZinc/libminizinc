include "lex_lesseq.mzn";

predicate fzn_lex_chain_lesseq_orbitope(array [int, int] of var int: xs, int: _kind) =
  if MZN__Orbitope then
    fzn_lex_chain_lesseq__orbitope(
      array1d(xs),
      card(index_set_1of2(xs)),
      _kind,
      true,
      not mzn_in_symmetry_breaking_constraint(),
    )
  else
    fzn_lex_chain_lesseq_orbitope__CP(xs, _kind)
  endif;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% SCIP 7.0.2, binary matrix, columns sorted
predicate fzn_lex_chain_lesseq__orbitope(
  array [int] of var int: matr,
  int: m,
  int: orbType,
  bool: resolveprop,
  bool: isModelCons,
);

predicate fzn_lex_chain_lesseq_orbitope__CP(array [int, int] of var int: a, int: kind) =
  let {
    set of int: is2 = index_set_2of2(a);
  } in (
    forall (j in is2 where j + 1 in is2) (lex_lesseq(col(a, j), col(a, j + 1))) /\
      if 1 == kind then
        forall (i in index_set_1of2(a)) (1 == sum(row(a, i)))
      elseif 2 == kind then
        forall (i in index_set_1of2(a)) (1 >= sum(row(a, i)))
      else
        true
      endif
  );

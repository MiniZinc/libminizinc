% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified forms, under the registry's names. See
% `fzn_lex_chain_lesseq_orbitope.mzn`.

include "fzn_bool_lex_chain_lesseq_orbitope_packing_reif.mzn";
include "fzn_bool_lex_chain_lesseq_orbitope_partition_reif.mzn";
include "fzn_bool_lex_chain_lesseq_reif.mzn";

predicate fzn_lex_chain_lesseq_orbitope_reif(
  array [int, int] of var int: xs,
  int: _kind,
  var bool: b,
) =
  let {
    int: columns = card(index_set_2of2(xs));
    array [int] of var int: m = array1d(xs);
    array [1..length(m)] of var bool: bs;
    constraint forall (i in 1..length(m)) (bool2int(bs[i]) = m[i]);
  } in if _kind = 1 then
    fzn_bool_lex_chain_lesseq_orbitope_partition_reif(bs, columns, b)
  elseif _kind = 2 then
    fzn_bool_lex_chain_lesseq_orbitope_packing_reif(bs, columns, b)
  else
    fzn_bool_lex_chain_lesseq_reif(bs, columns, b)
  endif;

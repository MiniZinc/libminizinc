% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` passes a magic integer for which extra row constraint to add: 0 for
% none, 1 for set-partitioning, 2 for set-packing. A magic integer is not a
% signature, and `_kind = 0` is already `bool_lex_chain_lesseq`, so the two that
% add something get a name each.
%
% They are `bool_`, not `int_`: `std` declares the matrix `var int` and then
% asserts it is 0/1, so the two spellings are channelled here — one `bool2int`
% per entry that is not already fixed or already a Boolean. That is a real cost
% on a model that decomposes anyway, and it is the price of the constraint being
% expressible at all: without it a solver has no orbitope to declare.

include "fzn_bool_lex_chain_lesseq.mzn";
include "fzn_bool_lex_chain_lesseq_orbitope_packing.mzn";
include "fzn_bool_lex_chain_lesseq_orbitope_partition.mzn";

predicate fzn_lex_chain_lesseq_orbitope(array [int, int] of var int: xs, int: _kind) =
  let {
    int: columns = card(index_set_2of2(xs));
    array [int] of var int: m = array1d(xs);
    array [1..length(m)] of var bool: bs;
    constraint forall (i in 1..length(m)) (bool2int(bs[i]) = m[i]);
  } in if _kind = 1 then
    fzn_bool_lex_chain_lesseq_orbitope_partition(bs, columns)
  elseif _kind = 2 then
    fzn_bool_lex_chain_lesseq_orbitope_packing(bs, columns)
  else
    fzn_bool_lex_chain_lesseq(bs, columns)
  endif;

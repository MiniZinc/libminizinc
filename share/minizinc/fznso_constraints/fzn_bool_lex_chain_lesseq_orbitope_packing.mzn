% The constraint `bool_lex_chain_lesseq_orbitope_packing`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `bool_lex_chain_lesseq_orbitope_packing` and the body below is dropped in
% favour of the propagator, or ship a file of this name with a decomposition of
% your own.

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

predicate fzn_bool_lex_chain_lesseq_orbitope_packing(list of var bool: xs, int: columns) =
  fzn_bool_lex_chain_lesseq(xs, columns) /\
    forall (i in 1..length(xs) div columns) (
      sum (j in 1..columns) (xs[(i - 1) * columns + j]) <= 1
    );

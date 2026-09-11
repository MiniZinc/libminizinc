% The constraint `bool_lex_chain_lesseq_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_lex_chain_lesseq_reif`
% and the body below is dropped in favour of the propagator, or ship a file of
% this name with a decomposition of your own.

predicate fzn_bool_lex_chain_lesseq_reif(
  list of var bool: xs,
  int: columns,
  var bool: b,
) =
  b <-> (
    if columns < 2 then
      true
    else
      let {
        array [1..length(xs) div columns, 1..columns] of var bool: m =
          array2d(1..length(xs) div columns, 1..columns, xs);
      } in forall (j in 1..columns - 1) (lex_lesseq(col(m, j), col(m, j + 1)))
    endif
  );

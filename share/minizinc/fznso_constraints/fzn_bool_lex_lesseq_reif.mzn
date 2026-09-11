% The constraint `bool_lex_lesseq_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_lex_lesseq_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_bool_lex_lesseq_reif(
  list of var bool: xs,
  list of var bool: ys,
  var bool: b,
) =
  b <-> lex_lesseq_std_decomposition(xs, ys);

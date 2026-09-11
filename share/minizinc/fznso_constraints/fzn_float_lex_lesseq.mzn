% The constraint `float_lex_lesseq`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `float_lex_lesseq` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_float_lex_lesseq(list of var float: xs, list of var float: ys) =
  lex_lesseq_std_decomposition(xs, ys);

% This file contains redefinitions of standard builtins that can be overridden
% by solvers.

predicate bool_clause_reif(array[int] of var bool: as,
                           array[int] of var bool: bs,
                           var bool: b) =
  clause(as,bs++[b]) /\
  forall (i in index_set(as)) (as[i] -> b) /\
  forall (i in index_set(bs)) (bs[i] \/ b);


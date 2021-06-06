% This file contains redefinitions of standard builtins for version 2.5.6
% that can be overridden by solvers.

predicate bool_clause_imp(array[int] of var bool: as, array[int] of var bool: bs, var bool: b) =
  clause(as,bs++[b]);

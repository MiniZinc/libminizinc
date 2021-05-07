% This file contains redefinitions of standard builtins for version 2.5.6
% that can be overridden by solvers.

predicate bool_clause_imp(
    array [int] of var bool: p ::promise_ctx_monotone,
    array [int] of var bool: n ::promise_ctx_antitone,
    var bool: c ::promise_ctx_antitone,
) =
    (
        sum(i in index_set(p))(bool2int(p[i]))
        - sum(i in index_set(n))(bool2int(n[i]))
        + length(n)
    ) >= bool2int(c);
    

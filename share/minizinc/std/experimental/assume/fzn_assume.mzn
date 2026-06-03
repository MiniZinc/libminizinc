/*
  Solver-overridable lowering of the `assume` predicate.

  A solver that supports assumptions provides its own `fzn_assume` in its solver
  library, which is then posted as a native FlatZinc constraint. This default
  decomposition warns the user (so they do not expect an unsatisfiable core to
  be reported) and posts the assumptions as ordinary hard constraints.
*/
predicate fzn_assume(array [int] of var bool: b :: promise_ctx_monotone) =
  if length(b) > 0 then
    trace(
      "Warning: the selected solver does not support assumptions ('fzn_assume'); the " ++
      "assumptions are posted as hard constraints and no unsatisfiable core will be reported.\n",
    )
  else
    true
  endif /\ forall(b);

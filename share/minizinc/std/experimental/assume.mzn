include "experimental/assume/fzn_assume.mzn";

/***
  @groupdef experimental.assume Assumptions

  An *assumption* interface for solvers that support solving under assumptions.
  The Boolean variables passed to \(\a assume\) are assumed to be true; a solver
  that supports assumptions may, when the model is unsatisfiable under these
  assumptions, report a subset of them as an unsatisfiable core. MiniZinc maps
  such a core back to the original expressions and prints it for inspection
  (look for the ``Unsatisfiable core`` comment in the solver output).
*/

/** @group experimental.assume
  Assume that the Boolean variables in \a b hold.

  A solver supporting assumptions treats these as assumptions rather than hard
  constraints, and may report a subset of them as an unsatisfiable core when the
  model is unsatisfiable under the assumptions. Fixed-true elements are dropped;
  a fixed-false element makes the model trivially unsatisfiable (and is reported
  to the user). If the selected solver has no native support, the assumptions are
  posted as hard constraints and a warning is issued.
*/
predicate assume(array [int] of var bool: b :: promise_ctx_monotone) =
  % Fixed-false assumptions make the model trivially unsatisfiable; report each.
  forall (i in index_set(b) where is_fixed(b[i]) /\ not fix(b[i])) (
    trace(
      "Warning: assumption b[\(i)] is fixed to false; the model is unsatisfiable under the " ++
      "given assumptions.\n",
      false,
    )
  ) /\
  % Drop fixed-true assumptions; pass the rest to the solver interface.
  fzn_assume([b[i] | i in index_set(b) where not (is_fixed(b[i]) /\ fix(b[i]))]);

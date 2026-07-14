include "subcircuit.mzn";

predicate fzn_circuit_opt_reif(array [int] of var opt int: xs, var bool: b) =
  b <->
    (
      subcircuit(
        array1d(index_set(xs), [if occurs(xs[i]) then deopt(xs[i]) else i endif | i in index_set(xs)]),
      ) /\
        forall (i in index_set(xs)) (occurs(xs[i]) -> deopt(xs[i]) != i)
    );

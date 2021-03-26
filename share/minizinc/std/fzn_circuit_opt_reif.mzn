include "subcircuit.mzn";

predicate fzn_circuit_opt_reif(array[int] of var opt int: x, var bool: b) = 
  b <-> (
    subcircuit(array1d(index_set(x),[if occurs(x[i]) then deopt(x[i]) else i endif |  i in index_set(x)]))
    /\ forall (i in index_set(x)) (occurs(x[i]) -> deopt(x[i]) != i));

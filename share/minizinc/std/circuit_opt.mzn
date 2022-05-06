include "fzn_circuit_opt.mzn";
include "fzn_circuit_opt_reif.mzn";

/** @group globals.graph
  Constrains the elements of \a x to define a circuit where \a x[\p i] = \p j means
  that \p j is the successor of \p i. Absent elements do not take part in the circuit.
*/
predicate circuit(array[$$E] of var opt $$E: x) = 
    fzn_circuit_opt(x);

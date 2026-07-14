include "fzn_circuit_opt.mzn";
include "fzn_circuit_opt_reif.mzn";

predicate fzn_circuit_opt(array [int] of var opt int: xs);

/** @group globals.graph
  Constrains the elements of \a xs to define a circuit where \a xs[\p i] = \p j means
  that \p j is the successor of \p i. Absent elements do not take part in the circuit.
*/
predicate circuit(array [$$E] of var opt $$E: xs) = fzn_circuit_opt(index2int(enum2int(xs)));

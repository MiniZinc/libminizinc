include "fzn_circuit.mzn";
include "fzn_circuit_reif.mzn";

/** @group globals.graph
  Constrains the elements of \a x to define a circuit where \a x[\p i] = \p j means
  that \p j is the successor of \p i.
*/
predicate circuit(array[$$E] of var $$E: x) = 
    fzn_circuit(x);

%-----------------------------------------------------------------------------%

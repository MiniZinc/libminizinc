include "fzn_circuit.mzn";
include "fzn_circuit_reif.mzn";

predicate fzn_circuit(array [int] of var int: xs);

/** @group globals.graph
  Constrains the elements of \a xs to define a circuit where \a xs[\p i] = \p j means
  that \p j is the successor of \p i.
*/
predicate circuit(array [$$E] of var $$E: xs) = fzn_circuit(index2int(enum2int(xs)));

%-----------------------------------------------------------------------------%

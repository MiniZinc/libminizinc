include "fzn_subcircuit.mzn";
include "fzn_subcircuit_reif.mzn";

/** @group globals.graph
  Constrains the elements of \a xs to define a subcircuit where \a xs[\p i] = \p j
  means that \p j is the successor of \p i and \a xs[\p i] = \p i means that \p i
  is not in the circuit.
*/
predicate subcircuit(array [$$E] of var $$E: xs) = fzn_subcircuit(index2int(enum2int(xs)));

%-----------------------------------------------------------------------------%

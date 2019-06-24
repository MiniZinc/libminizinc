include "fzn_circuit.mzn";
include "fzn_circuit_reif.mzn";

/** @group globals
  Constrains the elements of \a x to define a circuit where \a x[\p i] = \p j means
  that \p j is the successor of \p i.
*/
predicate circuit(array[int] of var int: x) = 
    fzn_circuit(x);

predicate circuit_reif(array[int] of var int: x, var bool: b) = 
    fzn_circuit_reif(x, b);

%-----------------------------------------------------------------------------%

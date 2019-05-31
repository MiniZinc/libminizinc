include "fzn_alternative.mzn";

/** @group globals.scheduling
Alternative constraint for optional tasks. Task (\a s0,\a d0) spans the
optional tasks (\a s[\p i],\a d[\p i]) in the array arguments
and at most one can occur
*/
predicate alternative(var opt int: s0, var int: d0,
                      array[int] of var opt int: s,
                      array[int] of var int: d) =
          assert(index_set(s) = index_set(d),
                 "alternative: index sets of third and fourth argument must be identical",
          fzn_alternative(s0, d0, s, d)
          );

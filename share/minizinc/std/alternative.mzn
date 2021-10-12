include "fzn_alternative.mzn";
include "fzn_alternative_reif.mzn";

/** @group globals.scheduling
Alternative constraint for optional tasks. The task with start time \a s0 and
duration \a d0 spans the
optional tasks with start times \a s[\p i] and durations \a d[\p i]),
and at most one of those tasks can occur.
*/
predicate alternative(var opt int: s0, var int: d0,
                      array[int] of var opt int: s,
                      array[int] of var int: d) =
          assert(index_set(s) = index_set(d),
              "alternative: index sets of third and fourth argument must be identical",
              fzn_alternative(s0, d0, s, d)
          );

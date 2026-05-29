include "fzn_alternative.mzn";
include "fzn_alternative_reif.mzn";

/** @group globals.scheduling
Alternative constraint for optional tasks. The task with start time \a span_start and
duration \a span_duration spans the
optional tasks with start times \a starts[\p i] and durations \a durations[\p i]),
and at most one of those tasks can occur.
*/
predicate alternative(
  var opt int: span_start,
  var int: span_duration,
  array [int] of var opt int: starts,
  array [int] of var int: durations,
) =
  assert(
    index_set(starts) = index_set(durations),
    "alternative: index sets of third and fourth argument must be identical",
    fzn_alternative(span_start, span_duration, starts, durations),
  );

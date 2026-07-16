include "fzn_alternative.mzn";
include "fzn_alternative_reif.mzn";

predicate fzn_alternative(
  var opt int: span_start,
  var int: span_duration,
  array [int] of var opt int: start,
  array [int] of var int: duration,
);

/** @group globals.scheduling
Alternative constraint for optional tasks. The task with start time \a span_start and
duration \a span_duration spans the
optional tasks with start times \a start[\p i] and duration \a duration[\p i]),
and at most one of those tasks can occur.
*/
predicate alternative(
  var opt int: span_start,
  var int: span_duration,
  array [int] of var opt int: start,
  array [int] of var int: duration,
) =
  assert(
    index_set(start) = index_set(duration),
    "alternative: index sets of third and fourth argument must be identical",
    fzn_alternative(span_start, span_duration, start, duration),
  );

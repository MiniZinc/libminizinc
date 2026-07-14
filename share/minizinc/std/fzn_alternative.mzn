include "span.mzn";

predicate fzn_alternative(
  var opt int: span_start,
  var int: span_duration,
  array [int] of var opt int: starts,
  array [int] of var int: durations,
) = sum (i in index_set(starts)) (occurs(starts[i])) = occurs(span_start) /\ span(span_start, span_duration, starts, durations);

include "span.mzn";

predicate fzn_alternative(
  var opt int: span_start,
  var int: span_duration,
  array [int] of var opt int: start,
  array [int] of var int: duration,
) = sum (i in index_set(start)) (occurs(start[i])) = occurs(span_start) /\ span(span_start, span_duration, start, duration);

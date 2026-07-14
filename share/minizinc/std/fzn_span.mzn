predicate fzn_span(
  var opt int: span_start,
  var int: span_duration,
  array [int] of var opt int: starts,
  array [int] of var int: durations,
) =
  (occurs(span_start) <-> exists (i in index_set(starts)) (occurs(starts[i]))) /\
    span_start = min_weak(starts) /\
    (absent(span_start) -> span_duration = 0) /\
    span_start ~+ span_duration = max_weak([starts[i] ~+ durations[i] | i in index_set(starts)]);

predicate fzn_span_reif(
  var opt int: span_start,
  var int: span_duration,
  array [int] of var opt int: start,
  array [int] of var int: duration,
  var bool: b,
) =
  b <->
    (
      (occurs(span_start) <-> exists (i in index_set(start)) (occurs(start[i]))) /\
        span_start = min(start) /\
        (absent(span_start) -> span_duration = 0) /\
        span_start ~+ span_duration = max([start[i] ~+ duration[i] | i in index_set(start)])
    );

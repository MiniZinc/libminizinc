include "fzn_span.mzn";
include "fzn_span_reif.mzn";

predicate fzn_span(
  var opt int: span_start,
  var int: span_duration,
  array [int] of var opt int: starts,
  array [int] of var int: durations,
);

/** @group globals.scheduling
  Span constraint for optional tasks. Task (\a span_start,\a span_duration) spans the optional
  tasks (\a starts[\p i],\a durations[\p i]) in the array arguments.
*/
predicate span(
  var opt int: span_start,
  var int: span_duration,
  array [$$E] of var opt int: starts,
  array [$$E] of var int: durations,
) =
  assert(
    index_sets_agree(starts, durations),
    "span: index sets of third and fourth argument must be identical",
  ) /\
    fzn_span(span_start, span_duration, index2int(starts), index2int(durations));

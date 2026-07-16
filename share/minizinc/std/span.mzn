include "fzn_span.mzn";
include "fzn_span_reif.mzn";

predicate fzn_span(
  var opt int: span_start,
  var int: span_duration,
  array [int] of var opt int: start,
  array [int] of var int: duration,
);

/** @group globals.scheduling
  Span constraint for optional tasks. Task (\a span_start,\a span_duration) spans the optional
  tasks (\a start[\p i],\a duration[\p i]) in the array arguments.
*/
predicate span(
  var opt int: span_start,
  var int: span_duration,
  array [$$E] of var opt int: start,
  array [$$E] of var int: duration,
) =
  assert(
    index_sets_agree(start, duration),
    "span: index sets of third and fourth argument must be identical",
  ) /\
    fzn_span(span_start, span_duration, index2int(start), index2int(duration));

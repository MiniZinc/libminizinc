include "fzn_span.mzn";
include "fzn_span_reif.mzn";

/** @group globals.scheduling
  Span constraint for optional tasks. Task (\a s0,\a d0) spans the optional
  tasks (\a s[\p i],\a d[\p i]) in the array arguments.
*/
predicate span(
  var opt int: s0,
  var int: d0,
  array [$$E] of var opt int: s,
  array [$$E] of var int: d,
) =
  assert(
    index_sets_agree(s, d),
    "span: index sets of third and fourth argument must be identical",
  ) /\
    fzn_span(s0, d0, index2int(s), index2int(d));

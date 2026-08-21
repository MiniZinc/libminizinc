% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std`'s pairwise helper is `int_no_overlap_nd` on two boxes, so it forwards
% there rather than keeping a decomposition of its own: a solver with the
% k-dimensional propagator gets it here too.

include "fzn_int_no_overlap_nd.mzn";

predicate fzn_geost_nonoverlap_k(
  array [int] of var int: x1,
  array [int] of int: w1,
  array [int] of var int: x2,
  array [int] of int: w2,
) = fzn_int_no_overlap_nd(length(x1), array1d(x1) ++ array1d(x2), array1d(w1) ++ array1d(w2));

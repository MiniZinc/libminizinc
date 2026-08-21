% The constraint `int_no_overlap_nd_nonstrict_reif`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `int_no_overlap_nd_nonstrict_reif` and the body below is dropped in favour of
% the propagator, or ship a file of this name with a decomposition of your own.

predicate fzn_int_no_overlap_nd_nonstrict_reif(
  int: dimensions,
  list of var int: position,
  list of var int: size,
  var bool: b,
) =
  b <-> (
    let {
      int: n = length(position) div dimensions;
    } in forall (b1, b2 in 1..n where b1 < b2) (
      exists (j in 1..dimensions) (
        let {
          int: p1 = (b1 - 1) * dimensions + j;
          int: p2 = (b2 - 1) * dimensions + j;
        } in size[p1] = 0 \/
          size[p2] = 0 \/
          position[p1] + size[p1] <= position[p2] \/
          position[p2] + size[p2] <= position[p1]
      )
    )
  );

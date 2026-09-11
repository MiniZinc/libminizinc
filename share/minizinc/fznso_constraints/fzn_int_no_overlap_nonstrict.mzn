% The constraint `int_no_overlap_nonstrict`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_no_overlap_nonstrict` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

% See `fzn_diffn.mzn`. This is the form in which a box of zero extent overlaps
% nothing, so it is a constraint of its own rather than a special case.

predicate fzn_int_no_overlap_nonstrict(
  list of var int: x,
  list of var int: dx,
  list of var int: y,
  list of var int: dy,
) =
  forall (i, j in index_set(x) where i < j) (
    dx[i] = 0 \/
      dx[j] = 0 \/
      dy[i] = 0 \/
      dy[j] = 0 \/
      x[i] + dx[i] <= x[j] \/
      y[i] + dy[i] <= y[j] \/
      x[j] + dx[j] <= x[i] \/
      y[j] + dy[j] <= y[i]
  );

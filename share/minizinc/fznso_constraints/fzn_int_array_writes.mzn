% The constraint `int_array_writes`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_array_writes` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% See `fzn_write.mzn`. Two writes to one position have to agree here, so this
% can fail; `int_array_writes_sequential` lets the later one win instead, which
% is why the two are separate constraints and only that one is functional.

predicate fzn_int_array_writes(
  list of var int: xs,
  int: offset,
  list of var int: index,
  list of var int: value,
  list of var int: result,
) =
  let {
    array [offset..offset + length(xs) - 1] of var int: x =
      array1d(offset..offset + length(xs) - 1, xs);
    array [offset..offset + length(xs) - 1] of var int: o =
      array1d(offset..offset + length(xs) - 1, result);
  } in forall (j in index_set(index)) (o[index[j]] = value[j]) /\
    forall (i in index_set(x)) (
      if forall (j in index_set(index)) (index[j] != i) then o[i] = x[i] else true endif
    );

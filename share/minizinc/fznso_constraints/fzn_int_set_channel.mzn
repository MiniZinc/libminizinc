% The constraint `int_set_channel`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_set_channel` and the body
% below is dropped in favour of the propagator, or ship a file of this name with
% a decomposition of your own.

% See `fzn_inverse.mzn` for what the offsets are and why a negative one is left
% to the decomposition.

predicate fzn_int_set_channel(
  list of var int: ints,
  int: int_offset,
  list of var set of int: sets,
  int: set_offset,
) =
  let {
    array [int_offset..int_offset + length(ints) - 1] of var int: x =
      array1d(int_offset..int_offset + length(ints) - 1, ints);
    array [set_offset..set_offset + length(sets) - 1] of var set of int: y =
      array1d(set_offset..set_offset + length(sets) - 1, sets);
  } in forall (i in index_set(x)) (x[i] in index_set(y)) /\
    forall (j in index_set(y)) (y[j] subset index_set(x)) /\
    forall (i in index_set(x), j in index_set(y)) (x[i] = j <-> i in y[j]);

predicate fzn_int_set_channel(
  array [int] of var int: ints,
  array [int] of var set of int: sets,
) =
  if length(ints) = 0 \/ length(sets) = 0 \/ min(index_set(ints)) < 0 \/
    min(index_set(sets)) < 0
  then
    forall (i in index_set(ints)) (ints[i] in index_set(sets)) /\
      forall (j in index_set(sets)) (sets[j] subset index_set(ints)) /\
      forall (i in index_set(ints), j in index_set(sets)) (ints[i] = j <-> i in sets[j])
  else
    fzn_int_set_channel(array1d(ints), min(index_set(ints)), array1d(sets),
                        min(index_set(sets)))
  endif;

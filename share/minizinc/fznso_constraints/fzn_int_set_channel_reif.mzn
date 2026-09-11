% The constraint `int_set_channel_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_set_channel_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_int_set_channel_reif(
  list of var int: ints,
  int: int_offset,
  list of var set of int: sets,
  int: set_offset,
  var bool: b,
) =
  b <-> (
    let {
      array [int_offset..int_offset + length(ints) - 1] of var int: x =
        array1d(int_offset..int_offset + length(ints) - 1, ints);
      array [set_offset..set_offset + length(sets) - 1] of var set of int: y =
        array1d(set_offset..set_offset + length(sets) - 1, sets);
    } in forall (i in index_set(x)) (x[i] in index_set(y)) /\
      forall (j in index_set(y)) (y[j] subset index_set(x)) /\
      forall (i in index_set(x), j in index_set(y)) (x[i] = j <-> i in y[j])
  );

predicate fzn_int_set_channel_reif(
  array [int] of var int: ints,
  array [int] of var set of int: sets,
  var bool: b,
) =
  fzn_int_set_channel_reif(array1d(ints), min(index_set(ints)), array1d(sets), min(index_set(sets)), b);

% The constraint `int_regular_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_regular_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% `std` branches on `fzn_regular_is_decomposed` and has a direct decomposition
% for the case where the solver has no propagator. The overlay never sets that
% flag — a solver may implement the plain form — so only the augmented-automaton
% construction is kept, and it is correct either way: it posts an ordinary
% `int_regular`, which a solver takes natively or decomposes as it would any
% other.
%
% The construction appends a marker symbol encoding `b`. States `1..states` are
% the original; `ACC` is the only accepting state and `DEAD` a live but
% non-accepting sink, so that a string rejected by a missing transition can
% still reach `ACC` with `b = false`.

include "fzn_int_regular.mzn";

predicate fzn_int_regular_reif(
  list of var int: xs,
  int: states,
  int: alphabet,
  int: alphabet_offset,
  list of int: transition,
  int: initial,
  set of int: accepting,
  var bool: b,
) =
  let {
    array [1..states, 1..alphabet] of int: d = array2d(1..states, 1..alphabet, transition);
    int: acc = states + 1;
    int: dead = states + 2;
    int: qq = states + 2;
    int: ss = alphabet + 2;
    array [1..qq, 1..ss] of int: dd =
      array2d(
        1..qq,
        1..ss,
        [
          if r <= states then
            if c <= alphabet then
              (if d[r, c] = 0 then dead else d[r, c] endif)
            elseif c = alphabet + 1 then
              (if r in accepting then 0 else acc endif)
            else
              (if r in accepting then acc else 0 endif)
            endif
          elseif r = dead then
            (if c <= alphabet then dead elseif c = alphabet + 1 then acc else 0 endif)
          else
            0
          endif |
            r in 1..qq,
            c in 1..ss,
        ],
      );
    array [int] of var int: y = xs ++ [alphabet_offset + alphabet + bool2int(b)];
  } in fzn_int_regular(y, qq, ss, alphabet_offset, array1d(dd), initial, {acc});

% The constraint `set_of_int_to_bool`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `set_of_int_to_bool` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

% See `fzn_inverse.mzn` for what the offset is and why a negative one is left to
% the decomposition.

predicate fzn_set_of_int_to_bool(var set of int: s, list of var bool: flags, int: offset) =
  forall (i in index_set(flags)) (flags[i] <-> offset + i - 1 in s);

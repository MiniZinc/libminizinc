% The constraint `set_of_int_subset_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `set_of_int_subset_reif` and
% this declaration is matched, or ship a file of this name carrying a
% decomposition of your own.
%
% There is no decomposition here. A FlatZinc builtin backs
% `set_of_int_subset_reif`, which is to say it is primitive: every way of
% writing what it means uses the operator it defines, so a body here would
% flatten straight back to this identifier and the compiler would not terminate.

predicate fzn_set_of_int_subset_reif(var set of int: s, var set of int: t, var bool: b) =
  abort(
    "set_of_int_subset_reif: this solver neither implements the constraint nor decomposes it",
  );

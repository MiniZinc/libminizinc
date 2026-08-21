% The constraint `int_in_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_in_reif` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `int_in_reif`, which
% is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_int_in_reif(var int: x, var set of int: values, var bool: b) =
  abort(
    "int_in_reif: this solver neither implements the constraint nor decomposes it",
  );

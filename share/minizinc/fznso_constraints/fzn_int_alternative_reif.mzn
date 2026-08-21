% The constraint `int_alternative_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_alternative_reif` and
% this declaration is matched, or ship a file of this name carrying a
% decomposition of your own.
%
% There is no decomposition here. No MiniZinc global or builtin lowers to
% `int_alternative_reif` yet, so there is no standard decomposition to carry
% across. A solver that declares it is matched; one that does not needs its own
% file.

predicate fzn_int_alternative_reif(
  list of var int: start,
  list of var int: duration,
  var int: span_start,
  var int: span_duration,
  var bool: b,
) =
  abort(
    "int_alternative_reif: this solver neither implements the constraint nor decomposes it",
  );

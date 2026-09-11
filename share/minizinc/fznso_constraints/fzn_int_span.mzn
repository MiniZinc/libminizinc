% The constraint `int_span`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_span` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. No MiniZinc global or builtin lowers to
% `int_span` yet, so there is no standard decomposition to carry across. A
% solver that declares it is matched; one that does not needs its own file.

predicate fzn_int_span(
  list of var int: start,
  list of var int: duration,
  var int: span_start,
  var int: span_duration,
) =
  abort(
    "int_span: this solver neither implements the constraint nor decomposes it",
  );

% The constraint `set_of_int_card`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `set_of_int_card` and this
% declaration is matched, or ship a file of this name carrying a decomposition
% of your own.
%
% There is no decomposition here. A FlatZinc builtin backs `set_of_int_card`,
% which is to say it is primitive: every way of writing what it means uses the
% operator it defines, so a body here would flatten straight back to this
% identifier and the compiler would not terminate.

predicate fzn_set_of_int_card(var set of int: s, var int: n) =
  abort(
    "set_of_int_card: this solver neither implements the constraint nor decomposes it",
  );

% The second declaration is for fixed arguments. MiniZinc monomorphises a
% predicate called from a generic function — `card` in `stdlib_set.mzn` is
% generic over its element type — and `enable_native_predicates` leaves those
% variants their body on purpose, since the compiler evaluates them. Without one
% here the fixed call widens to the declaration above and aborts, even though
% the solver implements the constraint perfectly well. Stating it in terms of
% the parameter `card` is not a decomposition: it is the same constraint, worked
% out at compile time.
predicate fzn_set_of_int_card(set of int: s, int: n) = n = card(s);

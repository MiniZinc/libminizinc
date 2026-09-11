% Nothing: a solver with set variables takes `std`'s own `set_intersect`,
% `set_union`, `set_diff` and `set_symdiff`, whose results it holds as set
% variables.
%
% The file exists so that `redefinitions.mzn`'s include resolves. A solver
% without set variables gets `fznso_nosets/`'s copy instead, which explains why
% the function form is what that path needs.

% `std`'s own `min`/`max` over a set variable, carried over unchanged: a solver
% holding the set has nothing cheaper to say, and the array of scaled indicators
% is what `redefinitions-2.0.mzn` routes to the registry's array extrema.
% `fznso_nosets/` replaces these; see the note in `redefinitions-2.1.1.mzn`.

function var $$E: min(var set of $$E: s) =
  let {
    var set of int: si = enum2int(s);
    var min(ub(si))..max(ub(si)): m =
      min([e + (max(ub(si)) + 1 - e) * (1 - (e in si)) | e in ub(si)]);
  } in to_enum_internal(enum_of(s), m);

function var $$E: max(var set of $$E: s) =
  let {
    var set of int: si = enum2int(s);
    var min(ub(si))..max(ub(si)): m =
      max([e + (min(ub(si)) - 1 - e) * (1 - (e in si)) | e in ub(si)]);
  } in to_enum_internal(enum_of(s), m);

% `set_of_int_symdiff` for a solver with no set variables.
%
% Stated over the result's own bits rather than as an equality against the set
% `std/nosets.mzn`'s function returns: MiniZinc narrows the domain of a named
% result *after* the operation has been decomposed, and the narrowing never
% reaches the bits. `fznso_set_functions.mzn` keeps the function form, which is
% what MiniZinc actually takes; this is what a consumer calling the registry
% predicate directly gets.

include "fznso_set_to_bools.mzn";
include "fzn_bool_array_xor.mzn";

predicate fzn_set_of_int_symdiff(
  var set of int: s,
  var set of int: t,
  var set of int: result,
) =
  let {
    array [int] of var bool: bs = set2bools(s);
    array [int] of var bool: bt = set2bools(t);
    array [int] of var bool: br = set2bools(result);
  } in forall (
    i in index_set(bs) union index_set(bt) union index_set(br),
  ) (
    % `fzn_bool_array_xor` rather than `r = s xor t`: bodies here are never
    % dropped, and the `xor` operator leaves a `bool_xor` call that the rewrite
    % layer no longer gets to lower. Odd parity of the three with `t` negated is
    % `r <-> s xor t`.
    fzn_bool_array_xor(
      [set2bools_at(br, i), set2bools_at(bs, i), not set2bools_at(bt, i)],
    )
  );

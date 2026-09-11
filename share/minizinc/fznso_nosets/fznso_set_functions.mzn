% The set operations as functions returning a set, exactly as `std/nosets.mzn`
% writes them.
%
% The registry names the result instead, and `fzn_set_of_int_intersect` and its
% siblings state that form. But MiniZinc introduces the result variable *before*
% it flattens the operation and narrows its domain *after*, so by the time the
% narrowing arrives the result has already become an array of Booleans and
% nothing re-applies it: `(s intersect t) == {}` then holds with `s` and `t`
% sharing a member. A solver with set variables is unaffected, because the
% narrowing lands in a declaration it enforces itself.
%
% Overriding the function is what `std/nosets.mzn` does and what keeps the
% ordering safe: the operation is evaluated where it is written, so the
% narrowing reaches the set before its bits exist. `redefinitions.mzn` includes
% this because nothing else always would, and `fznso_constraints/` carries an
% empty file of this name for a solver that has set variables.

include "fzn_bool_array_xor.mzn";
include "fznso_set_to_bools.mzn";

function var set of int: set_intersect(var set of int: x, var set of int: y)
  :: promise_total
  :: promise_commutative =
  let {
    array [int] of var bool: bx = set2bools(x);
    array [int] of var bool: by = set2bools(y);
    var set of (index_set(bx) intersect index_set(by)): z;
    array [int] of var bool: bz = set2bools(z);
    constraint forall (i in index_set(bz)) (bz[i] = (bx[i] /\ by[i]));
  } in z;

function var set of int: set_union(var set of int: x, var set of int: y)
  :: promise_total
  :: promise_commutative =
  let {
    array [int] of var bool: bx = set2bools(x);
    array [int] of var bool: by = set2bools(y);
    var set of (index_set(bx) union index_set(by)): z;
    array [int] of var bool: bz = set2bools(z);
    constraint
      forall (i in index_set(bx) union index_set(by)) (
        if (i in index_set(bx)) then
          if (i in index_set(by)) then bz[i] = (bx[i] \/ by[i]) else bz[i] = bx[i] endif
        else
          bz[i] = by[i]
        endif
      );
  } in z;

function var set of int: set_diff(var set of int: x, var set of int: y) :: promise_total =
  let {
    array [int] of var bool: bx = set2bools(x);
    array [int] of var bool: by = set2bools(y);
    var set of ub(x) diff lb(y): z;
    array [int] of var bool: bz = set2bools(z);
    constraint forall (i in ub(z)) (bz[i] = (bx[i] /\ (not by[i])));
  } in z;

function var set of int: set_symdiff(var set of int: x, var set of int: y)
  :: promise_total
  :: promise_commutative =
  let {
    array [int] of var bool: bx = set2bools(x);
    array [int] of var bool: by = set2bools(y);
    var set of (ub(x) diff lb(y)) union (ub(y) diff lb(x)): z;
    array [int] of var bool: bz = set2bools(z);
    constraint
      % `fzn_bool_array_xor` rather than the `xor` operator: bodies here are
      % never dropped, so a `bool_xor` call left behind reaches the solver
      % unlowered. Odd parity of the three with `y` negated is `z <-> x xor y`.
      forall (i in ub(z)) (
        fzn_bool_array_xor(
          [bz[i], bx[i] :: maybe_partial, not (by[i] :: maybe_partial)],
        )
      );
  } in z;

% The largest and smallest member, stated over the set's own bits.
%
% `redefinitions-2.1.1.mzn` writes these as `max`/`min` of the array
% `[e + (sentinel - e) * (1 - (e in s)) | e in ub(s)]`. Every element of that
% array is a *scaled indicator*, and MiniZinc names each one — a row per member
% on top of whatever the extremum itself costs. `linear/` does not pay it,
% because a Boolean there is already a 0/1 column and `e * b` is a term rather
% than a definition; behind this interface a `var bool` is a Boolean, so the
% scaling has to be said, and it is cheaper to say it once inside the rows that
% need it than once per member in a definition of its own.
%
% What each member is worth is that same expression, written inline: the
% member's value when it is in the set, and the sentinel when it is not. The
% result is at least all of them, and equal to the one a selector picks. The
% selector needs no separate tie to membership — picking a non-member forces the
% result down to the sentinel, which the lower bounds already rule out unless
% the set really is empty.
%
% The sentinel — one below the extent for `max`, one above for `min` — lies
% outside the declared domain of the result, there as here, so an empty set
% makes the constraint unsatisfiable rather than quietly answering with it.

function var $$E: max(var set of $$E: s) :: promise_total =
  let {
    var set of int: si = enum2int(s);
    set of int: ext = ub(si);
  } in if card(ext) = 0 then
    to_enum_internal(enum_of(s), 0)
  else
    let {
      array [int] of var bool: bs = set2bools(si);
      array [int] of int: member = [e | e in ext];
      int: lo = min(ext);
      int: hi = max(ext);
      int: reach = hi - lo + 1;
      var lo..hi: m;
      array [index_set(member)] of var bool: at;
      % At least every member.
      constraint
        forall (j in index_set(member)) (
          fzn_int_lin_le(
            [-1, member[j] - lo + 1],
            [m, bool2int(bs[member[j]])],
            -(lo - 1),
          )
        );
      % And no more than the one `at` picks.
      constraint
        forall (j in index_set(member)) (
          fzn_int_lin_le(
            [1, -(member[j] - lo + 1), reach],
            [m, bool2int(bs[member[j]]), bool2int(at[j])],
            lo - 1 + reach,
          )
        );
      constraint fzn_bool_lin_eq([1 | j in index_set(member)], at, 1);
    } in to_enum_internal(enum_of(s), m)
  endif;

function var $$E: min(var set of $$E: s) :: promise_total =
  let {
    var set of int: si = enum2int(s);
    set of int: ext = ub(si);
  } in if card(ext) = 0 then
    to_enum_internal(enum_of(s), 0)
  else
    let {
      array [int] of var bool: bs = set2bools(si);
      array [int] of int: member = [e | e in ext];
      int: lo = min(ext);
      int: hi = max(ext);
      int: reach = hi - lo + 1;
      var lo..hi: m;
      array [index_set(member)] of var bool: at;
      constraint
        forall (j in index_set(member)) (
          fzn_int_lin_le(
            [1, hi + 1 - member[j]],
            [m, bool2int(bs[member[j]])],
            hi + 1,
          )
        );
      constraint
        forall (j in index_set(member)) (
          fzn_int_lin_le(
            [-1, -(hi + 1 - member[j]), reach],
            [m, bool2int(bs[member[j]]), bool2int(at[j])],
            -(hi + 1) + reach,
          )
        );
      constraint fzn_bool_lin_eq([1 | j in index_set(member)], at, 1);
    } in to_enum_internal(enum_of(s), m)
  endif;

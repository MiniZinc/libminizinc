% The set operations and element, exactly as `std/nosets.mzn` writes them, under
% private names. The registry predicates beside them differ in shape — three
% arguments rather than a returned set, an explicit index offset — and adapt to
% these rather than restating them.

include "fznso_set_to_bools.mzn";

function var set of int: fznso_nosets_set_intersect(var set of int: x, var set of int: y)
  :: promise_total
  :: promise_commutative =
  let {
    array [int] of var bool: bx = set2bools(x);
    array [int] of var bool: by = set2bools(y);
    var set of (index_set(bx) intersect index_set(by)): z;
    array [int] of var bool: bz = set2bools(z);
    constraint forall (i in index_set(bz)) (bz[i] = (bx[i] /\ by[i]));
  } in z;

function var set of int: fznso_nosets_set_union(var set of int: x, var set of int: y)
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

function var set of int: fznso_nosets_set_diff(var set of int: x, var set of int: y) :: promise_total =
  let {
    array [int] of var bool: bx = set2bools(x);
    array [int] of var bool: by = set2bools(y);
    var set of ub(x) diff lb(y): z;
    array [int] of var bool: bz = set2bools(z);
    constraint forall (i in ub(z)) (bz[i] = (bx[i] /\ (not by[i])));
  } in z;

function var set of int: fznso_nosets_set_symdiff(var set of int: x, var set of int: y)
  :: promise_total
  :: promise_commutative =
  let {
    array [int] of var bool: bx = set2bools(x);
    array [int] of var bool: by = set2bools(y);
    var set of (ub(x) diff lb(y)) union (ub(y) diff lb(x)): z;
    array [int] of var bool: bz = set2bools(z);
    constraint
       forall (i in ub(z)) (bz[i] = ((bx[i] :: maybe_partial) xor (by[i] :: maybe_partial)));
  } in z;

predicate fznso_nosets_array_var_set_element(var int: idx, array [int] of var set of int: xs, var set of int: y)
  :: promise_total =
  let {
    constraint idx in {i | i in index_set(xs) where lb(xs[i]) subset ub(y) /\ lb(y) subset ub(xs[i])};
    set of int: sUB = array_union([ub(xs[i]) | i in dom(idx)]);
    set of int: sLB = array_intersect([lb(xs[i]) | i in dom(idx)]);
    constraint y subset sUB;
    constraint sLB subset y;
  } in forall (k in ub(y)) (
    set2bools(y)[k] ==
      if k in sUB then
        if k in sLB then
          true
        else
          array1d(
            lb(idx)..ub(idx),
            [if k in ub(xs[i]) then set2bools(xs[i])[k] else false endif | i in lb(idx)..ub(idx)],
          )[idx]
        endif
      else
        false
      endif
  ) /\
    forall (
      k in sUB diff ub(y),
    ) (
      if k in sLB then
        false %% fail the constraint
      else
        not array1d(
          lb(idx)..ub(idx),
          [if k in ub(xs[i]) then set2bools(xs[i])[k] else false endif | i in lb(idx)..ub(idx)],
        )[idx]
      endif
    );

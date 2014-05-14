function var set of int: reverse_map(array[int] of var bool: b);
function set of int: reverse_map(array[int] of bool: b) ::promise_total =
  { i | i in index_set(b) where b[i] };

array[int] of var bool: set2bools(var set of int: x) ::promise_total =
  if is_fixed(x) then set2bools(fix(x)) else
  let {
    array[min(ub(x))..max(ub(x))] of var bool: b;
    constraint forall (i in index_set(b) where not (i in ub(x))) (b[i]=false);
    constraint (x = reverse_map(b)) :: is_reverse_map;
  } in b
  endif;
array[int] of var bool: set2bools(var set of int: x, set of int: ubx) ::promise_total =
  if is_fixed(x) then set2bools(fix(x) intersect ubx) else
  let {
    array[min(ubx)..max(ubx)] of var bool: b;
    constraint forall (i in index_set(b) where not (i in ubx)) (b[i]=false);
    constraint (x = reverse_map(b)) :: is_reverse_map;
  } in b
  endif;
array[int] of bool: set2bools(set of int: x) ::promise_total =
  array1d(min(x)..max(x),[i in x | i in min(x)..max(x)]);
array[int] of bool: set2bools(set of int: x, set of int: ubx) ::promise_total =
  set2bools(x intersect ubx);

predicate set_eq(var set of int: x, var set of int: y) =
  if not has_ub_set(x) /\ not has_ub_set(y) then
    assert(false, "Cannot determine bounds of set variables")
  elseif not has_ub_set(x) then set_eq(y,x)
  else
  let {
    set of int: uby = if has_ub_set(y) then ub(y) else ub(x) endif;
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y,uby);
  } in forall (i in index_set(bx) union index_set(by)) (
    if not (i in index_set(bx)) then not by[i]
    elseif not (i in index_set(by)) then not bx[i]
    else bx[i]=by[i] endif
  )
  endif;

predicate set_eq_reif(var set of int: x, var set of int: y, var bool: b) =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
  } in b <-> forall (i in index_set(bx) union index_set(by)) (
    if not (i in index_set(bx)) then not by[i]
    elseif not (i in index_set(by)) then not bx[i]
    else bx[i]=by[i] endif
  );

predicate set_subset(var set of int: x, var set of int: y) =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
  } in forall (i in index_set(bx)) (
    if not (i in index_set(by)) then not bx[i]
    else bx[i] -> by[i] endif
  );

predicate set_subset_reif(var set of int: x, var set of int: y, var bool: b) =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
  } in b <-> forall (i in index_set(bx)) (
    if not (i in index_set(by)) then not bx[i]
    else bx[i] -> by[i] endif
  );

function var set of int: set_intersect(var set of int: x, var set of int: y) ::promise_total =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
    var set of (index_set(bx) intersect index_set(by)): z;
    array[int] of var bool: bz = set2bools(z);
    constraint forall (i in index_set(bz)) (
      bz[i] = (bx[i] /\ by[i])
    );
  } in z;

function var set of int: set_union(var set of int: x, var set of int: y) ::promise_total =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
    var set of (index_set(bx) union index_set(by)): z;
    array[int] of var bool: bz = set2bools(z);
    constraint forall (i in index_set(bx) union index_set(by)) (
      if (i in index_set(bx)) then
        if (i in index_set(by)) then
          bz[i] = (bx[i] \/ by[i])
        else
          bz[i] = bx[i]
        endif
      else
          bz[i] = by[i]
      endif
    );
  } in z;

function var set of int: set_diff(var set of int: x, var set of int: y) ::promise_total =
  let {
    array[int] of var bool: bx = set2bools(x);
    array[int] of var bool: by = set2bools(y);
    var set of (index_set(bx) diff index_set(by)): z;
    array[int] of var bool: bz = set2bools(z);
    constraint forall (i in index_set(bz)) (
      bz[i] = (bx[i] /\ (not by[i]))
    );
  } in z;

function var int: card(var set of int: x) ::promise_total =
  let {
    array[int] of var bool: bx = set2bools(x);
    var 0..length(bx) : c;
    constraint bool_lin_eq([1 | i in index_set(bx)],bx,c);
  } in c;

predicate set_in(var int: x, var set of int: y) =
  let {
    array[int] of var bool: by = set2bools(y);
  } in by[x];

predicate set_in_reif(var int: x, var set of int: y, var bool: b) =
  let {
    array[int] of var bool: by = set2bools(y);
  } in b <-> by[x];

function array[int] of var bool: setarray2bools(array[int] of var set of int: x) =
  if length(x)=0 then [] else
  set2bools(x[1])++setarray2bools([x[i]|i in 2..length(x)])
  endif;

annotation set_search(array[int] of var set of int: x, ann: a1, ann: a2, ann: a3) =
  bool_search(setarray2bools(x),a1,a2,a3);

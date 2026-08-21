% A set variable as the array of Booleans that says which members it holds, and
% the reverse map that reads a solution back out. Lifted from `std/nosets.mzn`,
% which this layer replaces.

predicate mzn_reverse_map_var(var set of int: x) =
  let { array [int] of var bool: b = set2bools(x) } in true;

function var set of int: reverse_map_ab2si(array [int] of var bool: b) :: output_only;

function set of int: reverse_map_ab2si(array [int] of bool: b) :: promise_total = {
  i |
    i in index_set(b) where b[i],
};

function array [int] of var bool: set2bools(var set of int: x) :: promise_total =
  if is_fixed(x) then
    set2bools(fix(x))
  elseif ub(x) = {} then
    let {
      array [int] of bool: b = [];
      constraint (x = reverse_map_ab2si(b)) :: is_reverse_map;
    } in []
  else
    let {
      array [int] of var bool: b =
        array1d(min(ub(x))..max(ub(x)), [set2bools_bit(x, i) | i in min(ub(x))..max(ub(x))]);
      constraint (x = reverse_map_ab2si(b)) :: is_reverse_map;
    } in b
  endif;

function array [int] of var bool: set2bools(var set of int: x, set of int: ubx) :: promise_total =
  if is_fixed(x) then
    set2bools(fix(x), ubx)
  elseif ub(x) = {} then
    let {
      array [int] of bool: b = [];
      constraint (x = reverse_map_ab2si(b)) :: is_reverse_map;
    } in []
  else
    let {
      array [int] of var bool: b0 = set2bools(x); %% Call in any case ?? TODO
      array [int] of var bool: b =
        array1d(
          min(ubx)..max(ubx),
          [if i in ubx then set2bools_bit(x, i) else false endif | i in min(ubx)..max(ubx)],
        );
    } in b
  endif;

function array [int] of bool: set2bools(set of int: x) :: promise_total =
  if x = {} then [] else array1d(min(x)..max(x), [i in x | i in min(x)..max(x)]) endif;

function array [int] of bool: set2bools(set of int: x, set of int: ubx) :: promise_total =
  if ubx = {} then [] else array1d(min(ubx)..max(ubx), [i in x | i in min(ubx)..max(ubx)]) endif;

function var bool: set2bools_bit(var set of int: x, int: i) :: promise_total =
  if i in ub(x) then let { var bool: bi } in bi else false endif;

function array [int] of var bool: setarray2bools(array [int] of var set of int: x) =
  if length(x) = 0 then [] else set2bools(x[1]) ++ setarray2bools([x[i] | i in 2..length(x)]) endif;

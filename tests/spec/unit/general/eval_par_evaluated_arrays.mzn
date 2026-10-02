/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/
% Par arrays passed to functions and evaluated again keep their values. Arrays whose elements
% are all evaluated are reused instead of copied, so this covers the cases where that could go
% wrong: elements replaced by coercions after construction, absent elements, views and reshaped
% arrays sharing elements, and empty arrays.

function float: first_float(array [int] of float: xs) = xs[min(index_set(xs))];
function int: int_sum(array [int] of int: xs) = sum(xs);
function int: occurring(array [int] of opt int: xs) = sum (x in xs) (deopt(x) default 0);
function string: joined(array [int] of string: xs) = concat(xs);
function int: set_cards(array [int] of set of int: xs) = sum (x in xs) (card(x));
function int: count2d(array [int, int] of int: xs) = sum(xs);
function array [int] of float: same_floats(array [int] of float: xs) = xs;
function array [int] of int: same_ints(array [int] of int: xs) = xs;

% Coercions replace elements after the literal is built
array [1..2] of float: fs = [1, 2.0];
array [1..2] of int: bs = [true, 3];
constraint assert(first_float(fs) = 1.0, "int2float coercion");
constraint assert(show(fs) = "[1.0, 2.0]", "show after coercion");
constraint assert(int_sum(bs) = 4, "bool2int coercion");
constraint assert(show(same_floats(fs)) = "[1.0, 2.0]", "int2float coercion through a function");
constraint assert(show(same_ints(bs)) = "[1, 3]", "bool2int coercion through a function");

% Absent elements
array [1..3] of opt int: os = [1, <>, 3];
constraint assert(occurring(os) = 4 /\ occurring(os) = 4, "opt array");
constraint assert(occurring([<>, <>]) = 0, "only absent");

% Strings, sets and empty arrays
array [1..3] of string: ss = ["a", "b", "c"];
constraint assert(joined(ss) = "abc" /\ joined(ss) = "abc", "strings");
array [1..2] of set of int: sets = [{1, 2}, 3..5];
constraint assert(set_cards(sets) = 5, "sets");
array [int] of int: none = [];
constraint assert(int_sum(none) = 0, "empty array");

% Views and reshapes share their elements with the original array
array [1..6] of int: xs = [i | i in 1..6];
array [1..2, 1..3] of int: grid = array2d(1..2, 1..3, xs);
array [int] of int: flat_grid = array1d(grid);
constraint assert(count2d(grid) = 21 /\ int_sum(flat_grid) = 21, "reshape");
constraint assert(int_sum(grid[2, ..]) = 15 /\ int_sum(row(grid, 1)) = 6, "slice");
constraint assert(int_sum(array1d(5..10, xs)) = 21, "array1d with index set");

% Arrays from data files
array [1..3] of int: ds;
ds = [4, 5, 6];
constraint assert(int_sum(ds) = 15 /\ int_sum(ds) = 15, "data array");

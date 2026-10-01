/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/
% The builtin arg_val, arg_val_weak and first_duplicate on par arrays of integers and enums
include "arg_val.mzn";

enum E = {P, Q, R, S};
enum F = {A, B, C};
array [3..6] of int: xs = array1d(3..6, [10, 20, 10, 30]);
array [E] of F: fs = [C, B, B, A];

constraint assert(arg_val(xs, 20) = 4 /\ arg_val(xs, 30) = 6, "non-1-based index set");
constraint assert(arg_val(xs, 10) = 3, "least index");
constraint assert(absent(arg_val_weak(xs, 99)), "absent");
constraint assert(not (arg_val(xs, 99) = 3), "undefined in relational context");
constraint assert(arg_val([-5, 0, 5], -5) = 1, "negative values");

% Enum-indexed arrays of enums return index values of the index enum
constraint assert(arg_val(fs, B) = Q /\ arg_val_weak(fs, A) = S, "enum index and values");
constraint assert(first_duplicate(fs) = B, "enum duplicate");

constraint assert(absent(first_duplicate([1, 2, 3])), "no duplicate");
constraint assert(first_duplicate(xs) = 10, "one duplicate");
constraint assert(first_duplicate([1, 2, 2, 1]) = 2, "second occurrence first");
constraint assert(first_duplicate([1, 2, 3, 3, 2, 2]) = 3, "several duplicates");

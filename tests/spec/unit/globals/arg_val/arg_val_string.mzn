/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/
% The builtin arg_val, arg_val_weak and first_duplicate on par arrays of strings
include "arg_val.mzn";

enum E = {P, Q, R, S};
array [E] of string: ids = ["a", "", "b", "a"];
array [3..5] of string: from3 = array1d(3..5, ["x", "y", "z"]);
array [int] of string: none = [];

% Present and absent keys; results are index values, not positions
constraint assert(arg_val(ids, "b") = R, "arg_val present");
constraint assert(arg_val_weak(ids, "b") = R, "arg_val_weak present");
constraint assert(absent(arg_val_weak(ids, "c")), "arg_val_weak absent");
constraint assert(arg_val(from3, "x") = 3 /\ arg_val(from3, "z") = 5, "non-1-based index set");
constraint assert(absent(arg_val_weak(from3, "w")), "non-1-based absent");
constraint assert(absent(arg_val_weak(none, "a")), "empty array");

% The empty string is an ordinary key
constraint assert(arg_val(ids, "") = Q, "empty string key");
constraint assert(absent(arg_val_weak(from3, "")), "empty string absent");

% Ties are broken by the least index
constraint assert(arg_val(ids, "a") = P, "least index");

% A missing value makes arg_val undefined, which is false in a relational context
constraint assert(not (arg_val(from3, "w") = 3), "undefined in relational context");

% first_duplicate: no, one and several repeated keys. With several, the result is
% the key whose second occurrence comes first.
constraint assert(absent(first_duplicate(from3)), "no duplicate");
constraint assert(absent(first_duplicate(none)), "no duplicate in empty array");
constraint assert(first_duplicate(ids) = "a", "one duplicate");
constraint assert(first_duplicate(["", "x", ""]) = "", "empty string duplicate");
constraint assert(first_duplicate(["a", "b", "b", "a"]) = "b", "second occurrence first");
constraint assert(first_duplicate(["a", "b", "a", "b"]) = "a", "second occurrence first");
constraint assert(first_duplicate(["a", "b", "c", "c", "b", "b"]) = "c", "several duplicates");

% Through a user function, as in a lookup helper
function opt E: lookup(array [E] of string: m, string: s) = arg_val_weak(m, s);
constraint assert([lookup(ids, s) | s in ["a", "b", "", "c"]] = [P, R, Q, <>], "via function");

% Arrays whose elements are not literals are evaluated afresh on every call
function opt int: pair(string: x, string: s) = arg_val_weak([x, "b"], s);
constraint assert(pair("a", "a") = 1 /\ absent(pair("c", "a")) /\ pair("c", "c") = 1, "non-literal");
constraint assert(pair("a", "b") = 2 /\ pair("b", "b") = 1, "non-literal ties");

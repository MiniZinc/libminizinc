/***
!Test
expected: !Result
  status: SATISFIED
***/

% Test the is_fixed/1 built-in.

constraint is_fixed(_) = false;

constraint is_fixed(true) = true;
constraint is_fixed(42) = true;
constraint is_fixed(3.141) = true;
constraint is_fixed("Hello World\n") == true;

constraint is_fixed({}) = true;
constraint is_fixed({1, 2, 3}) = true;
constraint is_fixed({true}) = true;
constraint is_fixed({4.0}) = true;

constraint is_fixed([]) = true;
constraint is_fixed([true, false, true]) = true;
constraint is_fixed([1, 2, 3]) = true;
constraint is_fixed([1.0, 2.0, 3.0]) = true;
constraint is_fixed([{1, 2, 3}, {4, 5, 6}]) = true;
constraint is_fixed(["abc", "def", "ghi"]) = true;

bool: pb = true;
int: pi = 3;
float: pf = 4.0;
set of int: psi = {1, 2, 3};

constraint is_fixed(pb) = true;
constraint is_fixed(pi) = true;
constraint is_fixed(pf) = true;
constraint is_fixed(psi) = true;

array[1..3] of bool: apb = [true, false, true];
array[1..3] of int: api = [1, 2, 3];
array[1..3] of float: apf = [1.0, 2.0, 3.0];
array[1..3] of set of int: apsi = [{}, {1}, {2, 3}];
array[1..3] of string: astr = ["foo", "bar", "baz"];

constraint is_fixed(apb) = true;
constraint is_fixed(api) = true;
constraint is_fixed(apf) = true;
constraint is_fixed(apsi) = true;
constraint is_fixed(astr) = true;

constraint is_fixed(apb[1]) = true;
constraint is_fixed(apb[pi]) = true;

var bool: vb;
var int: vi;
var set of 0..100: vsi;

constraint is_fixed(vb) = false;
constraint is_fixed(vi) = false;
constraint is_fixed(vsi) = false;

array[1..3] of var bool: avb;
array[1..3] of var int: avi;
array[1..3] of var set of 0..100: avsi;

constraint is_fixed(avb) = false;
constraint is_fixed(avi) = false;
constraint is_fixed(avsi) = false;

var bool: vb2 = true;
var int: vi2 = 42;
var set of 0..100: vsi2 = {1, 2, 3};

constraint is_fixed(vb2) = true;
constraint is_fixed(vi2) = true;
constraint is_fixed(vsi2) = true;

predicate test_int(var int: x) =
	( if is_fixed(x) then true else false endif );

constraint test_int(341) = true;
constraint test_int(vi) = false;
constraint test_int(vi2) = true;

predicate test_array_int(array[int] of var int: x) = 
	( if is_fixed(x) then true else false endif );

constraint test_array_int([1, 2, 3]) = true;
constraint test_array_int([1, _, 3]) = false;
constraint test_array_int(avi) = false;

solve satisfy;

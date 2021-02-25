/***
!Test
type: output-model
solvers: [gecode]
expected: !OutputModel dzn_output_array.ozn
***/

enum Foo = { A, B, C };

var bool: x;
array [Foo] of var bool: a :: add_to_output = array1d(Foo, [x, x, x]);
array [1..3] of var bool: b :: add_to_output = [x, x, x];
array [int] of var bool: c :: add_to_output = [x];

array [Foo] of bool: d :: output_only :: add_to_output = array1d(Foo, [fix(x), fix(x), fix(x)]);
array [int] of bool: e :: output_only :: add_to_output = [fix(x)];
array [2..3] of bool: f :: output_only :: add_to_output = array1d(2..3, [fix(x), fix(x)]);
array [int] of bool: g :: output_only :: add_to_output = [fix(x) | i in 1..3];

array [1..2, int] of bool: h :: output_only :: add_to_output = array2d(1..2, 2..3, [fix(x) | i in 1..4]);

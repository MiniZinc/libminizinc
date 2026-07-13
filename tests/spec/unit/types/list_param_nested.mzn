/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    a: 10
    b: 40
    c: 7
    d: 99
***/

% `list's nested inside arrays of structs and arrays of lists must be 1-based, and are checked
% even when the outer array's index set is undeclared. The outer (non-list) index set of a sized
% array is unaffected.
% array whose elements are tuples containing a list:
function int: arr_tup(array[int] of tuple(int, list of int): x) = x[1].2[1];
% array of list (array-of-array), inner lists 1-based:
function int: arr_list(array[int] of list of int: x) = x[2][1];
% sized array of list: the outer (non-list) index set is preserved:
function int: sized(array[7..8] of list of int: x) = min(index_set(x));
% list of list (both index sets 1-based):
function int: list_list(array[1..infinity] of list of int: x) = x[2][2];

var 0..100: a;
var 0..100: b;
var 0..100: c;
var 0..100: d;
constraint a = arr_tup([(1, [10, 20, 30]), (2, [40, 50])]);
constraint b = arr_list([[10, 20, 30], [40, 50, 60]]);
constraint c = sized(array1d(7..8, [[10, 20, 30], [40, 50, 60]]));
constraint d = list_list([[10, 20, 30], [98, 99, 100]]);
solve satisfy;

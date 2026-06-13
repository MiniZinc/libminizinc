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

% `list`s nested inside arrays of structs and arrays of lists are coerced 1-based.
% array whose elements are tuples containing a list:
function int: arr_tup(array[int] of tuple(int, list of int): x) = x[1].2[1];
% array of list (array-of-array), inner lists coerced:
function int: arr_list(array[int] of list of int: x) = x[2][1];
% sized array of list: the outer (non-list) index set is preserved:
function int: sized(array[7..8] of list of int: x) = min(index_set(x));
% list of list (outer list rebased, inner list coerced):
function int: list_list(array[1..infinity] of list of int: x) = x[2][2];

var 0..100: a;
var 0..100: b;
var 0..100: c;
var 0..100: d;
constraint a = arr_tup([(1, array1d(3..5, [10, 20, 30])), (2, array1d(7..8, [40, 50]))]);
constraint b = arr_list([array1d(3..5, [10, 20, 30]), array1d(7..9, [40, 50, 60])]);
constraint c = sized(array1d(7..8, [array1d(3..5, [10, 20, 30]), array1d(9..11, [40, 50, 60])]));
constraint d = list_list(array1d(3..4, [array1d(3..5, [10, 20, 30]), array1d(7..9, [98, 99, 100])]));
solve satisfy;

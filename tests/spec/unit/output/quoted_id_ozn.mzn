/***
!Test
type: output-model
solvers: [gecode]
expected: !OutputModel quoted_id_ozn.ozn
***/

function string: 'foo!'(int: x) = show(x);
string: 'bar!' = "bar";
output ['foo!'(10), 'bar!'];
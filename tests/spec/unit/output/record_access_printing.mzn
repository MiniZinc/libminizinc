/***
!Test
type: output-model
solvers: [gecode]
expected: !OutputModel record_access_printing.ozn
***/

record(var 1..2: x, var 1..2: 'x+'): r;
any: a :: output = r.x;
any: b :: output = r.'x+';
solve minimize a + b;

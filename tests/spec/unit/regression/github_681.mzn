/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    arr: [true]
***/

array[1..1] of var bool: arr;
type R = record(array[int] of var bool: a, array[int] of var bool: b);
function R: f() = (a: arr, b: []);
R: r_rec = f();
constraint r_rec.a[1];

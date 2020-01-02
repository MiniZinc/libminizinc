/***
!Test
expected:
- !Result
  solution: !Solution
    a: [1, 1, 1]
    b: false
    i: 4
    x: 1
- !Result
  solution: !Solution
    a: [2, 1, 1]
    b: false
    i: 4
    x: 1
- !Result
  solution: !Solution
    a: [2, 3, 3]
    b: false
    i: 4
    x: 1
- !Result
  solution: !Solution
    a: [10, 10, 10]
    b: false
    i: 6
    x: 100
***/

array [1..3] of var 1..10: a;
var bool: b;
var 4..6: i;
var 1..100: x;
constraint a[i] = x  <->  b;
solve satisfy;

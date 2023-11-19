/***
!Test
expected: 
- !Result
  solution: !Solution
    x: [true, false, false]
    y: false
    c: 2
- !Result
  solution: !Solution
    x: [false, false, false]
    y: false
    c: 3
- !Result
  solution: !Solution
    x: [true, true, false]
    y: false
    c: 1
- !Result
  solution: !Solution
    x: [false, true, false]
    y: false
    c: 2
- !Result
  solution: !Solution
    x: [true, false, true]
    y: false
    c: 1
- !Result
  solution: !Solution
    x: [false, true, true]
    y: false
    c: 1
- !Result
  solution: !Solution
    x: [false, false, true]
    y: false
    c: 2
- !Result
  solution: !Solution
    x: [true, false, false]
    y: true
    c: 1
- !Result
  solution: !Solution
    x: [false, true, false]
    y: true
    c: 1
- !Result
  solution: !Solution
    x: [true, true, false]
    y: true
    c: 2
- !Result
  solution: !Solution
    x: [false, false, true]
    y: true
    c: 1
- !Result
  solution: !Solution
    x: [true, false, true]
    y: true
    c: 2
- !Result
  solution: !Solution
    x: [false, true, true]
    y: true
    c: 2
- !Result
  solution: !Solution
    x: [true, true, true]
    y: true
    c: 3
solvers: [gecode]
***/
include "count_eq.mzn";

array [1..3] of var bool: x;
var bool: y;
var 0..3: c;

constraint c > 0;
constraint c = count(x, y);

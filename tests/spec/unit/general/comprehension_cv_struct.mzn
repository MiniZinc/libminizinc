/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 1
    z: [null]
***/

var 1..2: x;
any: z :: output = [
  let {
    var int: y = x;
  } in if i == 1 then (<>,) else (1,) endif
|
  i in 1..2
][x];
solve minimize x;

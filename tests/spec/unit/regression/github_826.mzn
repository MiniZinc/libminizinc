/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    v: [false]
***/

predicate p(array[$$E] of var bool: b) =
  let {
    var bool: zero = not exists(b);
  } in zero;
  
enum SWITCHES = S(1..1);
array[SWITCHES] of var bool: v;

constraint p(v);

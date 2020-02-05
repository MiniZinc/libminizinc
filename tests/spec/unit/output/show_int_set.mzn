/***
!Test
expected: !Result
  solution: !Solution:
    y:
    - '{}'
    - '7..infinity'
    - '{3,5}'
    - '{3,5} union 7..infinity'
    - '-1..1'
    - '{-1,0,1} union 7..infinity'
    - '{-1,0,1,3,5}'
    - '{-1,0,1,3,5} union 7..infinity'
    - '{-5,-3}'
    - '{-5,-3} union 7..infinity'
    - '{-5,-3,3,5}'
    - '{-5,-3,3,5} union 7..infinity'
    - '{-5,-3,-1,0,1}'
    - '{-5,-3,-1,0,1} union 7..infinity'
    - '{-5,-3,-1,0,1,3,5}'
    - '{-5,-3,-1,0,1,3,5} union 7..infinity'
    - '-infinity..-7'
    - '-infinity..-7 union {} union 7..infinity'
    - '-infinity..-7 union {3,5}'
    - '-infinity..-7 union {3,5} union 7..infinity'
    - '-infinity..-7 union {-1,0,1}'
    - '-infinity..-7 union {-1,0,1} union 7..infinity'
    - '-infinity..-7 union {-1,0,1,3,5}'
    - '-infinity..-7 union {-1,0,1,3,5} union 7..infinity'
    - '-infinity..-7 union {-5,-3}'
    - '-infinity..-7 union {-5,-3} union 7..infinity'
    - '-infinity..-7 union {-5,-3,3,5}'
    - '-infinity..-7 union {-5,-3,3,5} union 7..infinity'
    - '-infinity..-7 union {-5,-3,-1,0,1}'
    - '-infinity..-7 union {-5,-3,-1,0,1} union 7..infinity'
    - '-infinity..-7 union {-5,-3,-1,0,1,3,5}'
    - '-infinity..-7 union {-5,-3,-1,0,1,3,5} union 7..infinity'
***/

array [1..5] of set of int: x = [
  -infinity..-7, 
  {-5, -3}, 
  -1..1, 
  {3, 5}, 
  7..infinity
];

function set of int: s(bool: b, set of int: x) = if b then x else {} endif;

array [1..32] of string: y :: add_to_output = [
  show(
    s(a, x[1]) union 
    s(b, x[2]) union
    s(c, x[3]) union
    s(d, x[4]) union
    s(e, x[5])
  ) | a, b, c, d, e in [false, true]
];
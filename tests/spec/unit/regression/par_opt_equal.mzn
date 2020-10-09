/***
!Test
expected: !Result
  solution: !Solution
    a: true
    b: false
    c: true
***/

opt bool: x = <>;
opt bool: y = <>;

bool: a :: add_to_output = (x == y);
bool: b :: add_to_output = (x != y);
bool: c :: add_to_output = (x == <>);
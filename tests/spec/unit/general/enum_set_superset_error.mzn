/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

enum A = {P};
enum B = {Q};

bool: b :: output = A superset B;

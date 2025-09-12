/***
!Test
solvers: [gecode]
options:
  -Werror: true
expected: !Result
  status: SATISFIED
***/

enum Foo = {A};
output show({enum2int(x) | x in Foo});

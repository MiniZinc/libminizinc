/***
!Test
extra_files:
- enum_constructor_basic.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: !ConstrEnum
      constructor: F
      argument: A
***/

enum Foo = { A };
enum Bar = F(Foo) ++ { B };

Bar: x :: add_to_output;

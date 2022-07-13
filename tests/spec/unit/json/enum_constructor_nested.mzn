/***
!Test
extra_files:
- enum_constructor_nested.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: !ConstrEnum
      constructor: G
      argument: !ConstrEnum
        constructor: F
        argument: A
***/

enum Foo = { A };
enum Bar = F(Foo) ++ { B };
enum Qux = G(Bar) ++ { C };

Qux: x :: add_to_output;

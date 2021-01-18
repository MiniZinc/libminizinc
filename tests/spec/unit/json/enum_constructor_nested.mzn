/***
!Test
extra_files:
- enum_constructor_nested.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    x:
      c: G
      e:
        c: F
        e: A
***/

enum Foo = { A };
enum Bar = F(Foo) ++ { B };
enum Qux = G(Bar) ++ { C };

Qux: x :: add_to_output;

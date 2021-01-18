/***
!Test
extra_files:
- enum_constructor_basic_2.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    x:
      c: F
      e: A
***/

enum Foo = { A };
enum Bar = F(Foo) ++ { B };

Bar: x :: add_to_output;

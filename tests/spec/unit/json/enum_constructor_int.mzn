/***
!Test
extra_files:
- enum_constructor_int.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: !ConstrEnum
      constructor: Foo
      argument: 2
***/

enum Bar = Foo(1..3);

Bar: x :: add_to_output;

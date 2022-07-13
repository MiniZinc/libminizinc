/***
--- !Test
extra_files:
- anon_enum_json.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: !AnonEnum
      enumName: Foo
      value: 1
    y: !AnonEnum
      enumName: Foo
      value: 2
--- !Test
extra_files:
- anon_enum_json_err.json
solvers: [gecode]
expected: !Error
  regex: .*invalid enum object.*
***/

enum Foo = anon_enum(10);

Foo: x :: add_to_output;
Foo: y :: add_to_output;

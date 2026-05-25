/***
!Test
extra_files:
- alias_enum_json_input.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  c: LCORE
***/

enum Unit;
type Colour = Unit;
Colour: c ::output;

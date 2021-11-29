/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: 'aebdc'
--- !Test
solvers: [gecode]
options:
  only-sections: foo,bar
expected: !Result
  solution: !Solution
    _output_item: 'bdc'
--- !Test
solvers: [gecode]
options:
  not-sections: foo,bar
expected: !Result
  solution: !Solution
    _output_item: 'ae'
***/

constraint output_to_section("default", "a");
constraint output_to_section("foo", "b");
constraint output_to_section("bar", "c");
constraint output_to_section("foo", "d");
constraint output_to_section("default", "e");

/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: true
    b: false
    c: true
***/

bool: a :: add_to_output = exists([|false, <> | <>, true|]);
bool: b :: add_to_output = exists([|<>, false | <>, <>|]);
bool: c :: add_to_output = exists([<>, true]);

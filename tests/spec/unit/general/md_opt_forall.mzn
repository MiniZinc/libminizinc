/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: true
    b: false
    c: true
***/

bool: a :: add_to_output = forall([|true, <> | <>, true|]);
bool: b :: add_to_output = forall([|true, <> | false, <>|]);
bool: c :: add_to_output = forall([<>, <>]);

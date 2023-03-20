/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    foo_int: 1
    foo_str: "test"
    foo_enum: B
    bar_int: 2
    bar_str: "pred"
    bar_enum: C
    baz_tup: [3, "func"]
    baz_rec: {"x": B}
***/

enum A = {B,C};

$T: foo(tuple($T): x) = x.1;
any: foo_int :: output = foo((1,));
any: foo_str :: output = foo(("test",));
any: foo_enum :: output = foo((B,));


$T: bar(record($T: t): x) = x.t;
any: bar_int :: output = bar((t: 2));
any: bar_str :: output = bar((t: "pred"));
any: bar_enum :: output = bar((t: C,));

function $X: baz($X: x) = x;

any: baz_tup ::output = baz((3, "func"));
any: baz_rec ::output = baz((x: B,));


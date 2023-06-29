/***
!Test
expected: !Result
  solution: !Solution
    x: true
    a: true
    b: true
    c: true
    p: true
***/

test foo(opt $T: x) = true;
any: x :: output = foo(<>);

test bar(opt $T, opt $T) = true;
any: a :: output = bar(1, <>);
any: b :: output = bar(<>, 1);
any: c :: output = bar(<>, <>);

test qux(tuple($T), opt $T) = true;
any: p :: output = qux((1,), <>);

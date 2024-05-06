/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    bar: null
    biz: !!set {B}
***/

enum foo = {A, B, C};
opt set of foo: bar :: output;
set of foo: biz :: output = bar default {B};

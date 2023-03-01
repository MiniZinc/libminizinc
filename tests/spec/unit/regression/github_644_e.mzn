/***
!Test
solvers:
- gecode
type: solve
expected: !Result
  solution: !Solution
    x: null
    y: null
    z: null
    p: 3
    q: 1
    r: 4
***/

% Par versions of ~+, ~- and ~* were missing

opt 1..2: a = <>;
opt 2..3: x :: output = a ~+ 1;
opt 0..1: y :: output = a ~- 1;
opt 2..4: z :: output = a ~* 2;

opt 1..2: b = 2;
opt 2..3: p :: output = b ~+ 1;
opt 0..1: q :: output = b ~- 1;
opt 2..4: r :: output = b ~* 2;

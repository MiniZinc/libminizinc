/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    p: 5
    q: null
    t1: 1
    t2: null
    n: [1, 2]
    na: null
    proj: [1, null]
***/

% Accessing a field of an optional struct yields an optional value: if the struct is absent, so
% is the field. (Same rule as array access with an absent index.)
opt record(int: x): present = (x: 5);
opt record(int: x): missing = <>;

opt int: p :: output = present.x;
opt int: q :: output = missing.x;

opt tuple(int, bool): tup = (1, true);
opt tuple(int, bool): notup = <>;
opt int: t1 :: output = tup.1;
opt int: t2 :: output = notup.1;

% A struct-typed field of an optional struct becomes an optional struct.
opt record(tuple(int, int): t): nested = (t: (1, 2));
opt record(tuple(int, int): t): nestedAbsent = <>;
opt tuple(int, int): n :: output = nested.t;
opt tuple(int, int): na :: output = nestedAbsent.t;

% Projecting a field out of an array of optional records.
array [1..2] of opt record(int: x): recs = [(x: 1), <>];
array [1..2] of opt int: proj :: output = recs.x;

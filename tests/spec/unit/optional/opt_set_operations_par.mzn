/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    in_: [true, true, true, true, true, true, false, true, false, true, true, false, false, true, true]
    sub: [true, true, true, true, true, true, true, true, true, true, true, false, true, false, true, true, false, false, true, true, true, false, false, false, true]
    sup: [true, true, true, true, true, true, true, false, false, false, true, true, true, false, false, true, true, false, true, false, true, true, true, true, true]
    un: [!!set {}, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}, !!set {}, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}, !!set {1}, !!set {1}, !!set {1}, !!set {1, 2}, !!set {1, 2}, !!set {2}, !!set {2}, !!set {1, 2}, !!set {2}, !!set {1, 2}, !!set {1, 2}, !!set {1, 2}, !!set {1, 2}, !!set {1, 2}, !!set {1, 2}]
    sd: [!!set {}, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}, !!set {}, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}, !!set {1}, !!set {1}, !!set {}, !!set {1, 2}, !!set {2}, !!set {2}, !!set {2}, !!set {1, 2}, !!set {}, !!set {1}, !!set {1, 2}, !!set {1, 2}, !!set {2}, !!set {1}, !!set {}]
    it: [null, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {1}, !!set {}, !!set {1}, !!set {}, !!set {1}, !!set {2}, !!set {}, !!set {}, !!set {2}, !!set {2}, !!set {1, 2}, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}]
    df: [null, null, null, null, null, !!set {}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {1}, !!set {1}, !!set {}, !!set {1}, !!set {}, !!set {2}, !!set {2}, !!set {2}, !!set {}, !!set {}, !!set {1, 2}, !!set {1, 2}, !!set {2}, !!set {1}, !!set {}]
    cd: [null, 0, 1, 1, 2]
    mn: [null, 1, 2, 1]
    ai: [null, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {}, !!set {1}, !!set {}, !!set {1}, !!set {}, !!set {1}, !!set {2}, !!set {}, !!set {}, !!set {2}, !!set {2}, !!set {1, 2}, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}]
    au: [!!set {}, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}, !!set {}, !!set {}, !!set {1}, !!set {2}, !!set {1, 2}, !!set {1}, !!set {1}, !!set {1}, !!set {1, 2}, !!set {1, 2}, !!set {2}, !!set {2}, !!set {1, 2}, !!set {2}, !!set {1, 2}, !!set {1, 2}, !!set {1, 2}, !!set {1, 2}, !!set {1, 2}, !!set {1, 2}]
    ai_empty: null
***/

% Par versions of the opt set operations agree with the var versions
array [1..5] of opt set of int: v = [<>, {}, {1}, {2}, {1, 2}];
array [1..3] of opt int: oe = [<>, 1, 2];
set of int: I = 1..5;
array [int] of bool: in_ :: output = [oe[k] in v[j] | k in 1..3, j in I];
array [int] of bool: sub :: output = [v[i] subset v[j] | i, j in I];
array [int] of bool: sup :: output = [v[i] superset v[j] | i, j in I];
array [int] of set of int: un :: output = [v[i] union v[j] | i, j in I];
array [int] of set of int: sd :: output = [v[i] symdiff v[j] | i, j in I];
array [int] of opt set of int: it :: output = [v[i] intersect v[j] | i, j in I];
array [int] of opt set of int: df :: output = [v[i] diff v[j] | i, j in I];
array [int] of opt int: cd :: output = [card(v[i]) | i in I];
array [int] of opt int: mn :: output = [min(v[i]) | i in I where v[i] != {}];
array [int] of opt set of int: ai :: output = [array_intersect([v[i], v[j]]) | i, j in I];
array [int] of set of int: au :: output = [array_union([v[i], v[j]]) | i, j in I];
array [1..0] of opt set of int: emp = [];
opt set of int: ai_empty :: output = array_intersect(emp);

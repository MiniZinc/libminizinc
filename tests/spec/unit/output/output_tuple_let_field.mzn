/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    o: [false, 0]
***/

% As in output_tuple_let.mzn, but another variable is defined from a field of o. The output model
% then adds o's reverse mapper (a tuple literal, not a call) after flattening (crash).
tuple(var bool, var int): o :: output = let { var bool: b; var 0..3: d = 0; constraint not b } in (b, d);
var bool: p = o.1;
solve satisfy;

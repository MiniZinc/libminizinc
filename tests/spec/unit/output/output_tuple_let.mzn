/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    o: [false, 0]
***/

% The fields of the output tuple are defined in a let, so the output model computes o from a
% reverse mapper. Removing enum types from its declaration also removed the tuple type (crash).
tuple(var bool, var int): o :: output = let { var bool: b; var 0..3: d = 0; constraint not b } in (b, d);
solve satisfy;

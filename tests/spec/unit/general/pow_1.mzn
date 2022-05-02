/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

% Test for pow with par arguments (builtin)

test check_pow(int: x, int: y, int: z) = 
    assert(pow(x, y) = z, "Expected pow(\(x), \(y)) = \(z) but got \(pow(x, y)).");

constraint check_pow(0, 0, 1);
constraint check_pow(0, 1, 0);
constraint check_pow(0, 3, 0);
constraint check_pow(1, 0, 1);
constraint check_pow(1, 1, 1);
constraint check_pow(1, 3, 1);
constraint check_pow(1, -1, 1); % 1 div pow(1, 1) = 1 div 1 = 1
constraint check_pow(1, -2, 1); % 1 div pow(1, 2) = 1 div 1 = 1
constraint check_pow(2, 0, 1);
constraint check_pow(2, -1, 0); % 1 div pow(2, 1) = 1 div 2 = 0
constraint check_pow(2, -2, 0); % 1 div pow(2, 2) = 1 div 4 = 0

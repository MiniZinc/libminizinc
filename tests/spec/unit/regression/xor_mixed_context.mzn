/***
!Test
expected: !Result
  solution: !Solution
    x: [1, 0, 1]
***/

array[1..3] of var 0..1: x; % 0 = Knight, 1 = Knave

constraint (x[1] = 1) xor (sum(x) = 1);
constraint (x[2] = 1) xor (sum(x) = 2);
constraint (x[3] = 1) xor (sum(x) = 3);

solve satisfy;
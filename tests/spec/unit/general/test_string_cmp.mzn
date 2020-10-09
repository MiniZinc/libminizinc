/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    xs: [1, 2, 3, 1, 2, 3]
***/

array [1..6] of var 1..3: xs;

array [1..6] of string: ss = ["a", "b", "c", "a", "b", "c"];

constraint
	forall (i, j in 1..6 where i < j /\ ss[i] < ss[j]) (xs[i] < xs[j]);

solve satisfy;

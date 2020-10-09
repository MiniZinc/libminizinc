/***
--- !Test
solvers: [gecode, chuffed]
expected:
- !Result
  solution: !Solution
    q: [1, 7, 4, 6, 8, 2, 5, 3]
- !Result
  solution: !Solution
    q: [1, 7, 5, 8, 2, 4, 6, 3]
***/

int: n = 8;

array [1..n] of var 1..n: q;

predicate 
    noattack(int: i, int: j, var int: qi, var int: qj) =
	qi     != qj     /\
	qi + i != qj + j /\
	qi - i != qj - j;

constraint
    forall (i in 1..n, j in i+1..n) (
        noattack(i, j, q[i], q[j])
    );

solve ::
	int_search(
		q,
		smallest,
		indomain,
		complete
	)
	satisfy;

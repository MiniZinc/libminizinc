/***
--- !Test
solvers: [gecode, chuffed]
expected: !Result
  solution: !Solution
    q: [8, 4, 1, 3, 6, 2, 7, 5]
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
		input_order,
		indomain_max,
		complete
	)
	satisfy;

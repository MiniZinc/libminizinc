% latin_squares.mzn
include "globals.mzn";

int: n = 3;
set of int: N = 1..n;
array[N, N] of var N: X;

constraint :: "ADRows"
    forall (i in N)
        (alldifferent(row(X, i)) :: "AD(row \(i))");
constraint :: "ADCols"
    forall (j in N)
        (alldifferent(col(X, j)) :: "AD(col \(j))");

constraint :: "LLRows"
    forall (i in 1..n-1)
        (lex_less(row(X, i), row(X, i+1)) :: "LL(rows \(i) \(i+1))");
constraint :: "LGCols"
    forall (j in 1..n-1)
        (lex_greater(col(X, j), col(X, j+1)) :: "LG(cols \(j) \(j+1)");

solve satisfy;

output [ show2d(X) ];

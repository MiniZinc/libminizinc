int: n; % number of queens
set of int: ROW = 1..n;
set of int: COL = 1..n;
array[int] of int: q; % col of queen in each row

test check(bool: b,string: s) =
  if b then true else trace_stdout("ERROR: "++s++"\n",false) endif;

output [
  if check(index_set(q)=1..n, "ERROR: array q should have index set 1..\(n)")
  /\ forall(i in 1..n)(check(q[i] in 1..n, "ERROR: q[\(i)] should have a value in 1..\(n)"))
  /\ forall(r1, r2 in 1..n where r1 < r2)
           (check(q[r1] != q[r2],
            "queens in rows \(r1) and \(r2) are on same column\n")
            /\
            check(q[r1]+r1 != q[r2]+r2,
            "queens in rows \(r1) and \(r2) are on same up diagonal\n")
            /\
            check(q[r1]-r1 != q[r2]-r2,
            "queens in rows \(r1) and \(r2) are on same down diagonal\n")
           )
  then "CORRECT: All constraints hold"    
  else "INCORRECT" endif ];

int: n; % 拉丁方的大小
array[1..n,1..n,1..n] of var bool: a;

predicate atmostone(array[int] of var bool:x) =
          forall(i,j in index_set(x) where i < j)(
            (not x[i] \/ not x[j]));
predicate exactlyone(array[int] of var bool:x) =
          atmostone(x) /\ exists(x);

constraint forall(i,j in 1..n)(
             exactlyone(k in 1..n)(a[i,j,k]) /\
             exactlyone(k in 1..n)(a[i,k,j]) /\
             exactlyone(k in 1..n)(a[k,i,j])
           );
solve satisfy;
output [ if fix(a[i,j,k]) then
            show(k) ++ if j == n then "\n" else " " endif 
         else "" endif | i,j,k in 1..n ];

int: n;
set of int: W = 1..n;
int: m;
set of int: T = 1..2*m;
array[W,T] of int: profit;
array[W,W] of bool: compatible;

array[W] of var T: task;
array[T] of var opt W: worker;

include "inverse.mzn";
constraint inverse(task,worker);
constraint forall(t in T where t != max(T))
         (compatible[worker[t],worker[t+1]]);

var int: obj = sum(w in W)(profit[w,task[w]]);

solve maximize obj;

output [ if fix(occurs(worker[t])) then show(worker[t]) else " " endif ++ " "
       | t in T ] 
       ++ [ show(task), "\n" ]
       ++ [ "Profit: ", show(obj), "\n"]
       ;

predicate inverse(array[int] of var opt int: f, array[int] of var opt int: g) = 
          forall(i in index_set(f), j in index_set(g)) (
             (occurs(f[i]) /\ deopt(f[i]) = j) <-> (occurs(g[j]) /\ deopt(g[j]) = i));

n = 7;
m = 4;

profit = [| 1,4,6,8, 8,4,3,4
          | 3,5,2,4, 9,5,8,2
          | 2,8,3,5, 6,3,7,1
          | 2,5,3,4, 8,6,1,3
          | 3,4,4,4, 6,6,6,3
          | 2,2,1,2, 4,3,3,1
          | 2,6,5,3, 3,3,3,3 |];

compatible = [| false, false, true,  true,  false, false, true
              | false, false, true,  true,  false, true,  false
              | true,  false, false, true,  true,  false, true
              | true,  true,  true,  false, true,  false, false
              | false, false, true,  true,  false, true,  true
              | false, true,  false, false, true,  false, true
              | true,  false, true,  false, true,  true,  false |];

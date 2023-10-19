% ignore
int:n;
set of int: NODES = 1..n;
array [NODES,NODES] of bool: adj;
bool: t = true;
bool: f = false;
n = 4;
int:count=sum([ 1 | i,j,k in NODES where 
          trace("+", i<j /\j<k /\ adj[i,j] /\ adj[i,k] /\ adj[j,k]) ]);
adj = [| false, true,  true,  false
       | true,  false, true,  false
       | true,  true,  false, true
       | false, false, true,  false |];
constraint trace("\n",true);
solve satisfy;

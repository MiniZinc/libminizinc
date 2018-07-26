% ignore
int:n;
set of int: NODES = 1..n;
array [NODES,NODES] of bool: adj;
bool: t = true;
bool: f = false;
n = 4;
adj = [| false, true,  true,  false
       | true,  false, true,  false
       | true,  true,  false, true
       | false, false, true,  false |];
solve satisfy;
  int: count = sum ([ 1 | i,j in NODES where i < j  /\ adj[i,j],
                          k in NODES where trace("+", j < k /\ adj[i,k] /\ adj[j,k])]);

% ignore
constraint trace("\n",true);

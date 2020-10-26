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
int: count = sum( i,j in NODES where i < j /\ adj[i,j])(
       sum([trace("("++show(i)++","++show(j)++","++show(k)++")",1) | 
             k in NODES where  j < k /\ adj[i,k] /\ adj[j,k]]));
% ignore
constraint trace("\n",true);

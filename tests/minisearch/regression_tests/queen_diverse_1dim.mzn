% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear

% n-queens example in Zinc using CP techniques
% By Reza Rafeh July 2005
% MiniZinc version
% Peter Stuckey September 30 2006

int: n = 8;

array [1..n] of var 1..n: q;


constraint
    alldifferent(q)     % rows
    /\ 
    alldifferent(i in 1..n)(q[i] + i-1)     % diagonals
    /\
    alldifferent(i in 1..n)(q[i] + n-i);

constraint q[1] = 2;
%constraint q[2] = 1;

include "alldifferent.mzn";


% ----------------------------------------------------------
% SEARCH 
% ----------------------------------------------------------

include "minisearch.mzn";
int: k = 4; % the number of diverse solutions
bool: verbose = false;

solve ::
	int_search(
		q,
		first_fail,
		indomain_min,
		complete
	)
search greedy_maxDiverseKset(q,k,verbose);


function ann: greedy_maxDiverseKset(array[int] of var int: Vars, int: K, bool: verbose) =
  let { array[int] of int: Store = [0 | x in 1..K*length(Vars)];
        int: vl = length(Vars);
        int: L = 0 % current length
  } in
  next() /\ commit() /\ print("Found solution\n") /\ print() /\
  % initialize the store
  repeat(i in 1..vl) ( % BUG: length(Vars)) does not work
       Store[i] := sol(q[i])  % BUG: q[i] works, Vars[i] does not
  ) /\ 

  repeat(
     if L = K-1 then break else L := L+1 endif /\     
     print("L = "++show(L)++"\n") /\
     scope( 
          let {var int: obj;} in        
          post(obj = sum(j in 1..L,i in index_set(Vars))(Store[(j-1)*vl + i] != Vars[i])) /\
          maximize_bab(obj) /\ commit() /\ 
          print("Found new diverse solution with obj = "++show(sol(obj))++"\n") /\ 
          print() /\ 
            % store this newly found solution
          repeat(i in 1..vl) ( % BUG: length(Vars)) does not work
            Store[(L-1)* vl + i] := sol(q[i])  % BUG: q[i] works, Vars[i] does not
          )  
     ) /\
     % print the current Store
     if verbose then 
       repeat (l in 1..L) (
         print("store\(l)=[") /\
         repeat (i in 1..vl) (
             print("\(Store[(l-1)*vl + i]), ")
         )
         /\ print("];\n")
       ) 
     else skip endif
  )
;



output	["8 queens, CP version:\n"] ++
	[	if fix(q[i]) == j then "Q " else ". " endif ++
	 	if j = n then "\n" else "" endif
	|	i, j in 1..n
	]
;

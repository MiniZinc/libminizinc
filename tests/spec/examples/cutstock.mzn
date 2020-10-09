/***
!Test
expected:
- !Result
  solution: !Solution
    items:
    - [0, 0, 0]
    - [0, 0, 0]
    - [0, 0, 0]
    - [0, 0, 0]
    - [0, 0, 2]
    - [0, 2, 0]
    - [1, 0, 1]
    - [1, 0, 1]
    obj: 4
    objective: 4
    pieces: [0, 0, 0, 0, 1, 1, 1, 1]
- !Result
  solution: !Solution
    items:
    - [0, 0, 3]
    - [0, 2, 0]
    - [1, 0, 1]
    - [1, 0, 1]
    - [0, 0, 0]
    - [0, 0, 0]
    - [0, 0, 0]
    - [0, 0, 0]
    obj: 4
    objective: 4
    pieces: [1, 1, 1, 1, 0, 0, 0, 0]
***/

%------------------------------------------------------------------------------%
% cutstock.mzn
% Jakob Puchinger <jakobp@cs.mu.oz.au>
% December 2007
% vim: ft=zinc ts=4 sw=4 et tw=0
%
% The cutting stock problem: Kantorovitch formulation for colgen.
%
%------------------------------------------------------------------------------%

int: L; % stock length
int: K; % upper bound on number of stock pieces
int: N; % number of stock pieces

array[1..N] of int: i_length;
array[1..N] of int: i_demand;

array[1..K] of var 0..1: pieces;
array[1..K, 1..N] of var int: items;

constraint
    forall(k in 1..K, i in 1..N)( items[k,i] >=0 );

solve ::int_search(array1d(1..K*N,items)++pieces,input_order,indomain_min,complete) minimize
	sum([ pieces[k] | k in 1..K]);

constraint 
	forall(i in 1..N)
	(
	  sum( [ items[k, i] | k in 1..K] ) >= i_demand[i]
	);

constraint
    forall( k in 1..K ) (
        sum(i in 1..N)( items[k,i] * i_length[i] ) <= pieces[k] * L
    );

    % required for showing the objective function
var int: obj;
constraint
	obj = sum([ pieces[k] | k in 1..K]);

output 
[ "Cost = ",  show( obj ), "\n" ] ++ 
[ "Pieces = \n\t" ] ++ [show(pieces)] ++ [ "\n" ] ++ 
[ "Items = \n\t" ] ++
[ show(items[k, i]) ++ if k = K then "\n\t" else " " endif |
    i in 1..N, k in 1..K ] ++
[ "\n" ];


% data:
N = 3;
K = sum(i_demand);
L = 10;
i_length = [7, 5, 3];
i_demand = [2, 2, 4];


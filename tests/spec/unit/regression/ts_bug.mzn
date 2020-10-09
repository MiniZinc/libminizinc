/***
!Test
solvers: [gecode]
expected:
- !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    cost: 48
    s: [4, 1, 2, 3, 6, 5]
    dur: [!!set {2, 4}, !!set {1, 2, 4}, !!set {1, 3, 4}, !!set {1, 3, 4}, !!set {1, 3}, !!set {1, 3}]
    bef: [!!set {}, !!set {2, 4}, !!set {1, 2, 4}, !!set {1, 2, 3, 4}, !!set {1, 2, 3, 4}, !!set {1, 2, 3, 4}]
    aft: [!!set {1, 2, 3, 4}, !!set {1, 2, 3, 4}, !!set {1, 3, 4}, !!set {1, 3, 4}, !!set {1, 3}, !!set {}]
    a: [!!set {1, 2}, !!set {2, 3}, !!set {3, 4}, !!set {2, 4}, !!set {1, 3}, !!set {1, 4}]
- !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    cost: 48
    s: [4, 1, 2, 3, 6, 5]
    dur: [!!set {2, 4}, !!set {1, 2, 4}, !!set {1, 3, 4}, !!set {1, 3, 4}, !!set {1, 3}, !!set {1, 3}]
    bef: [!!set {}, !!set {2, 4}, !!set {1, 2, 4}, !Range 1..4, !Range 1..4, !Range 1..4]
    aft: [!Range 1..4, !Range 1..4, !!set {1, 3, 4}, !!set {1, 3, 4}, !!set {1, 3}, !!set {}]
    a: [!Range 1..2, !Range 2..3, !Range 3..4, !!set {2, 4}, !!set {1, 3}, !!set {1, 4}]
- !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    cost: 48
    s: [5, 3, 2, 1, 4, 6]
    dur: [!!set {1, 3}, !!set {1, 3, 4}, !!set {1, 2, 4}, !!set {1, 2, 4}, !!set {1, 4}, !!set {1, 4}]
    bef: [!!set {}, !!set {1, 3}, !!set {1, 3, 4}, !Range 1..4, !Range 1..4, !Range 1..4]
    aft: [!Range 1..4, !Range 1..4, !!set {1, 2, 4}, !!set {1, 2, 4}, !!set {1, 4}, !!set {}]
    a: [!Range 1..2, !Range 2..3, !Range 3..4, !!set {2, 4}, !!set {1, 3}, !!set {1, 4}]
- !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    cost: 48
    s: [4, 1, 2, 3, 5, 6]
    dur: [!!set {2, 4}, !!set {1, 2, 4}, !!set {1, 3, 4}, !!set {1, 3, 4}, !!set {1, 4}, !!set {1, 4}]
    bef: [!!set {}, !!set {2, 4}, !!set {1, 2, 4}, !Range 1..4, !Range 1..4, !Range 1..4]
    aft: [!Range 1..4, !Range 1..4, !!set {1, 3, 4}, !!set {1, 3, 4}, !!set {1, 4}, !!set {}]
    a: [!Range 1..2, !Range 2..3, !Range 3..4, !!set {2, 4}, !!set {1, 3}, !!set {1, 4}]
***/

% mzn2fzn and minizinc aborted on this model with an error message about
% being unable to compute set bounds.  This was fixed in r8782.
% (This is derived from the talent scheduling model in the MiniZinc
% benchmarks.)

include "all_different.mzn";

int: numActors; % number of actors
int: numScenes; % numer of scenes

%-- Types ---------------------------------------------------------------------
set of int: Actors = 1..numActors;
set of int: Scenes = 1..numScenes;

array[Actors,Scenes] of 0..1: ia; % 01 definition of actors in scenes
array[Scenes] of set of Actors: a :: add_to_output = 
     [ { j | j in Actors where ia[j,i] == 1} | i in Scenes] ; % actors for each scene
array[Scenes] of int: d; % duration of each scene
array[Actors] of int: c; % cost of each actor

numScenes = 6;
numActors = 4;
ia = [|
1,0,0,0,1,1|
1,1,0,1,0,0|
0,1,1,0,1,0|
0,0,1,1,0,1|];
c = [1,1,1,1];
d = [1,2,3,4,5,6];

%-- Decision variables --------------------------------------------------------

array[Scenes] of var Scenes: s :: add_to_output; % schedule of scenes

%-- Auxilliary variables ------------------------------------------------------

array[Scenes] of var set of Actors: bef :: add_to_output; % actors appearing before time t
array[Scenes] of var set of Actors: aft :: add_to_output; % actors appearing after time t
array[Scenes] of var set of Actors: dur :: add_to_output; % actors on set at time t

var int: cost :: add_to_output;

%-- Constraints ---------------------------------------------------------------

constraint all_different(s); % each scene scheduled once

constraint
        bef[1] = {} /\       % no actors before time 1
        aft[numScenes] = {} /\       % no actors after time numScenes
	forall(t in 1..numScenes-1)(
               bef[t+1] = a[s[t]] union bef[t]   /\ 
               aft[t] = a[s[t+1]] union aft[t+1] );

constraint 
	dur[1] = a[s[1]] /\   
        forall(t in 2..numScenes-1)( 
		 dur[t] = bef[t+1] intersect aft[t]
	) /\
        dur[numScenes] = a[s[numScenes]];

constraint cost = sum(i in Scenes)(
      	       	     sum(j in Actors)(
		     	   c[j] * d[s[i]] * bool2int(j in dur[i])
		         )
                  );   

%-- symmetry breaking constraint
constraint s[1] < s[numScenes];

%-- Solving objective and solution output -------------------------------------

solve :: int_search(s, first_fail, indomain, complete)
      minimize cost;

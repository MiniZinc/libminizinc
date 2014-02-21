% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_fdlp
% RUNS ON mzn_mer_std_fd

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
array[Scenes] of set of Actors: a = 
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

array[Scenes] of var Scenes: s; % schedule of scenes

%-- Auxilliary variables ------------------------------------------------------

array[Scenes] of var set of Actors: bef; % actors appearing before time t
array[Scenes] of var set of Actors: aft; % actors appearing after time t
array[Scenes] of var set of Actors: dur; % actors on set at time t

var int: cost;

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

output ["cost = ", show(cost), ";\ns = ", show(s), 
       ";\ndur = ", show(dur), 
       ";\nbef = ", show(bef), 
       ";\naft = ", show(aft),
       ";\na = ", show(a),
       ";\n"]; 

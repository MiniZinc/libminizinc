/***
!Test
expected:
- !Result
  solution: !Solution
    sokPosns: [14, 13, 8, 7, 8, 9, 4, 5, 10, 1, 1, 1]
    stime: 9
***/

% Regression test for bug #335.
% Previous versions of CPX found this model to be unsatisfiable.
% A slightly later version found a solution but it was not optimal
% (stime = 11, rather than the optimal stime = 9.)

int: w;                          % width of board
int: n;                          % board positions 1, ..., w, w+1, ...., 2w, ..., 
constraint assert(n mod w == 0, "board must be rectangular");  
set of int: POS = 1..n; 
int: stps;                       % max steps in solution
set of int: STEPS = 1..stps;

int: nw;                         % number of walls
set of int: WALLS = 1..nw;
array[WALLS] of POS: walls;      % wall positions
int: nc;                         % number of crates + goals 
set of int: CRATES = 1..nc;  
set of POS: goals;               % goal positions;   
array[CRATES] of POS: crates;    % initial crate positions
POS: pInit;                      % initial sokoban position

set of int: MOVES = {-w,-1,1,w}; % possible moves

include "alldifferent.mzn";

array[STEPS] of var POS: sokPosn; % sokoban position
array[STEPS] of var MOVES: move;  % sokoban position
array[STEPS,CRATES] of var POS: cratePosns; 
var STEPS: stime :: add_to_output;                 % time of solution


%% initial positions
constraint sokPosn[1] = pInit;
constraint forall(c in CRATES)(cratePosns[1,c] = crates[c]);

%% no overlap of crates, walls and sokoban
constraint forall(s in STEPS)(
	       alldifferent(walls ++ [sokPosn[s]] ++ [cratePosns[s,c] | c in CRATES])
	   );

%% at the end all crates are in a position
constraint forall(c in CRATES)(
	       cratePosns[stime,c] in goals
	   );

% legal move at each step
constraint forall(s in 1..stps-1)(
	       sokPosn[s+1] - sokPosn[s] = move[s]
	   );

% crate pushing
constraint forall(s in 1..stps-1, c in CRATES)(
	      cratePosns[s+1,c] = cratePosns[s,c] + bool2int(sokPosn[s+1] == cratePosns[s,c]) * move[s]
	   );

solve minimize stime;

% output for test case comparison
array[STEPS] of var POS: sokPosns :: add_to_output;
constraint forall (s in 1..stime) (sokPosns[s] = sokPosn[s]);
constraint forall (s in (stime+1)..stps) (sokPosns[s] = 1); % Just fill the rest with 1 for consistency

w = 5;
n = 15;
stps = 12;

nw = 0;
walls = [];

nc = 2;
crates = [7,9];
goals = {6,15};
pInit = 14;

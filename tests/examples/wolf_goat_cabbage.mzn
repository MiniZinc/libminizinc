% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
%-----------------------------------------------------------------------------%
% The wolf, goat, cabbage problem
%
% A farmer has to take a wolf, goat and cabbage across a bridge
% He can only take one thing at a time
% The wolf and goat can't be left together alone (without the farmer)
% The goat and cabbage can't be left alone together
%-----------------------------------------------------------------------------%

% horizon is the maximum number of steps that might be required in the plan
int: horizon = 20;

%-----------------------------------------------------------------------------%

% -1..1 represent the three locations:
% -1 is on the left bank of the river
% 0 is on the bridge
% 1 is on the right bank
% A boolean, for each object, time and location,
% holds if the object is at the location at that time
array [1..horizon,-1..1] of var bool: wolf;
array [1..horizon,-1..1] of var bool: goat;
array [1..horizon,-1..1] of var bool: cabbage;
array [1..horizon,-1..1] of var bool: farmer;

%-----------------------------------------------------------------------------%

% Things can only move from bank to bridge, or from bridge to bank, in one step
constraint forall(t in 2..horizon,loc1 in -1..1,loc2 in -1..1)
                  (wolf[t-1,loc1] /\ wolf[t,loc2] -> loc1-loc2<2 /\ loc2-loc1<2);

constraint forall(t in 2..horizon,loc1 in -1..1,loc2 in -1..1)
                  (goat[t-1,loc1] /\ goat[t,loc2] -> loc1-loc2<2 /\ loc2-loc1<2);

constraint forall(t in 2..horizon,loc1 in -1..1,loc2 in -1..1)
                  (cabbage[t-1,loc1] /\ cabbage[t,loc2] -> loc1-loc2<2 /\ loc2-loc1<2);

constraint forall(t in 2..horizon,loc1 in -1..1,loc2 in -1..1)
                  (farmer[t-1,loc1] /\ farmer[t,loc2] -> loc1-loc2<2 /\ loc2-loc1<2);

% Can't leave wolf and goat, or goat and cabbage, together
constraint (forall(t in 1..horizon,loc in -1..1)
           ( (wolf[t,loc] /\ goat[t,loc] -> farmer[t,loc])
             /\
             (goat[t,loc] /\ cabbage[t,loc] -> farmer[t,loc])
           ) );

% The wolf is somewhere at each time point, and is only in one place
constraint forall(t in 1..horizon)
           (exists(loc in -1..1)
             (wolf[t,loc] /\ forall(loc2 in -1..1 where loc2!=loc)
                                   (not wolf[t,loc2])
             )
           );
% Similarly for the goat, cabbage and farmer...
constraint forall(t in 1..horizon)
           (exists(loc in -1..1)
             (goat[t,loc] /\ forall(loc2 in -1..1 where loc2!=loc)
                                   (not goat[t,loc2])
             )
           );
constraint forall(t in 1..horizon)
           (exists(loc in -1..1)
             (cabbage[t,loc] /\ forall(loc2 in -1..1 where loc2!=loc)
                                   (not cabbage[t,loc2])
             )
           );
constraint forall(t in 1..horizon)
           (exists(loc in -1..1)
             (farmer[t,loc] /\ forall(loc2 in -1..1 where loc2!=loc)
                                   (not farmer[t,loc2])
             )
           );

% The wolf can ony be on the bridge if:
% (a) The farmer is on the bridge, and neither the goat nor the cabbage is
% (b) The farmer was previously on the same bank as the wolf
% (c) The farmer goes subsequently to the same bank as the wolf
constraint forall(t in 2..horizon-1)
           ((wolf[t,0] -> farmer[t,0] /\ not goat[t,0] /\ not cabbage[t,0]
              /\ (wolf[t-1,1] <-> farmer[t-1,1]) /\ not farmer[t-1,0]
              /\ (wolf[t+1,1] <-> farmer[t+1,1]) /\ not farmer[t+1,0])
           /\
% Similarly for the cabbage
           (cabbage[t,0] -> farmer[t,0] /\ not goat[t,0] /\ not wolf[t,0]
               /\ (cabbage[t-1,1] <-> farmer[t-1,1]) /\ not farmer[t-1,0]
               /\ (cabbage[t+1,1] <-> farmer[t+1,1]) /\ not farmer[t+1,0])
           /\
% and for the goat
           (goat[t,0] -> farmer[t,0] /\ not wolf[t,0] /\ not cabbage[t,0]
                /\ (goat[t-1,1] <-> farmer[t-1,1]) /\ not farmer[t-1,0]
                /\ (goat[t+1,1] <-> farmer[t+1,1]) /\ not farmer[t+1,0])
           );

% The animals all start on the right bank and finish on the left bank
constraint
      (wolf[1,1] /\ goat[1,1] /\ cabbage[1,1]
      /\
      wolf[horizon,-1] /\ goat[horizon,-1] /\ cabbage[horizon,-1]
      );


solve ::bool_search(array1d(1..3*horizon,wolf)++array1d(1..3*horizon,goat)++array1d(1..3*horizon,cabbage)++array1d(1..3*horizon,farmer), input_order, indomain_max, complete) satisfy;

%-----------------------------------------------------------------------------%

output [
        "wolf    : ", show(wolf),    "\n",
        "goat    : ", show(goat),    "\n",
        "cabbage : ", show(cabbage), "\n",
        "farmer  : ", show(farmer),  "\n"
];

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%


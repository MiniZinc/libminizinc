include "partition_set.mzn";
int: weeks;    set of int: WEEK = 1..weeks; 
int: groups;   set of int: GROUP = 1..groups;
int: size;     set of int: SIZE = 1..size;
int: ngolfers = groups*size; 
set of int: GOLFER = 1..ngolfers;

array[WEEK,GROUP] of var set of GOLFER: Sched;

% constraints
constraint 
    forall (i in 1..weeks-1) (
               Sched[i,1] < Sched[i+1,1]
    ) /\
    forall (i in WEEK, j in GROUP) (
           card(Sched[i,j]) = size
        /\ forall (k in j+1..groups) (
%                Sched[i,j] < Sched[i,k]
%             /\ 
                Sched[i,j] intersect Sched[i,k] = {}
           )
    ) /\
    forall (i in WEEK) (
          partition_set([Sched[i,j] | j in GROUP], GOLFER)
%       /\ forall (j in 1..groups-1) (
%              Sched[i,j] < Sched[i,j+1]
%          )
    ) /\
    forall (i in 1..weeks-1, j in i+1..weeks) (
        forall (x,y in GROUP) (
            card(Sched[i,x] intersect Sched[j,y]) <= 1
        )
    );
% symmetry
  constraint 
    % Fix the first week %
    forall (i in GROUP, j in SIZE) (
        ((i-1)*size + j) in Sched[1,i]
    ) /\
    % Fix first group of second week %
    forall (i in SIZE) (
        ((i-1)*size + 1) in Sched[2,1]
    ) /\
    % Fix first 'size' players
    forall (w in 2..weeks, p in SIZE) (
        p in Sched[w,p]
    );
  
solve satisfy;

output [ show(Sched[i,j]) ++ " " ++
         if j == groups then "\n" else "" endif |
         i in WEEK, j in GROUP ];

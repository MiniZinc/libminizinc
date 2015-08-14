%=============================================================================%
% Vehicle Routing problem
% CP formulation
%
% Andrea Rendl
% March 2015
%============================================================================%

include "globals.mzn";
include "combinators.mzn";

int: N; % number of nodes in the MIP model
int: Capacity; % maximum capacity of each vehicle

int: nbVehicles = N; % maximum number of vehicles
int: nbCustomers = N;
int: timeBudget = sum (i in 1..N) (max([ Distance[i,j] | j in 1..N]) ); % the maximal of time that we got

set of int: VEHICLE = 1..nbVehicles;
set of int: CUSTOMER = 1..nbCustomers;
set of int: TIME = 0..timeBudget;
set of int: LOAD = 0..Capacity;

% the last nodes represent the start and end node for each vehicle (the depot)
set of int: NODES = 1..nbCustomers+2*nbVehicles; 
set of int: DEPOT_NODES = nbCustomers+1..nbCustomers+2*nbVehicles;
set of int: START_DEPOT_NODES = nbCustomers+1..nbCustomers+nbVehicles;
set of int: END_DEPOT_NODES = nbCustomers+nbVehicles+1..nbCustomers+2*nbVehicles;

array[1..N] of int: Demand; % demand from MIP model
array[NODES] of int: demand = [  % adapting demand to giant tour representation
  if i <= N then 
    Demand[i]
  else 
    0
  endif
| i in NODES]; 

array[1..N+1, 1..N+1] of int: Distance; % distance matrix from MIP model
% adapting distance matrix to giant tour representation
array[NODES, NODES] of int: distance = array2d(NODES,NODES,[           
  if i<=nbCustomers /\ j <= nbCustomers then 
    Distance[i+1,j+1]
  elseif i<=nbCustomers /\ j>nbCustomers then % depot-customer
    Distance[1,i+1]
  elseif j<=nbCustomers /\ i>nbCustomers then % customer-depot
    Distance[j+1,1]
  else 
    Distance[1,1] % depot-depot
  endif
   | i,j in NODES ]);  

% =================================================%
% Variables
% =================================================%

array[NODES] of var NODES: successor; 
array[NODES] of var NODES: predecessor; 
array[NODES] of var VEHICLE: vehicle; % which vehicle visits which customer?
array[NODES] of var LOAD: load; % load when arriving at node n in NODES
array[NODES] of var TIME: arrivalTime; % the time at which the vehicle serving node i will arrive at i
var int: traveltime;

% =================================================%
% Constraints
% =================================================%

% ------ initialization constraints ---- %
% predecessor of start nodes are end nodes
constraint 
   forall(n in (nbCustomers+2..nbCustomers+nbVehicles)) (
     predecessor[n] = n + nbVehicles-1
   );
constraint 
   predecessor[nbCustomers+1] = nbCustomers+2*nbVehicles;

% successors of end nodes are start nodes
constraint 
   forall(n in (nbCustomers+nbVehicles+1..nbCustomers+2*nbVehicles-1)) (
     successor[n] = n-nbVehicles+1 
   );
constraint
   successor[nbCustomers+2*nbVehicles] = nbCustomers+1;

% associate each start/end nodes with a vehicle
constraint 
   forall(n in START_DEPOT_NODES) (
     vehicle[n] = n-nbCustomers
   );
   
constraint 
   forall(n in END_DEPOT_NODES) (
     vehicle[n] = n-nbCustomers-nbVehicles
   );

% vehicles leave the depot at time zero
constraint 
   forall(n in START_DEPOT_NODES) (
     arrivalTime[n] = 0 
   );

% vehicle load when starting at the depot
constraint 
   forall(n in START_DEPOT_NODES) (
     load[n] = 0 % demand[n]
   );


% ------- predecessor/successor constraints --- %
constraint
   forall(n in NODES) (
      successor[predecessor[n]] = n
   );
constraint
   forall(n in NODES) (
      predecessor[successor[n]] = n
   );

% alldiff + subtour elimination constraints
constraint 
   circuit(successor);
constraint 
   circuit(predecessor);


% ---- vehicle constraints ------------- %

% vehicle of node i is the same as the vehicle for the predecessor
constraint 
   forall(n in CUSTOMER) (
      vehicle[predecessor[n]] = vehicle[n]
   );
constraint 
   forall(n in CUSTOMER) (
      vehicle[successor[n]] = vehicle[n]
   );


% ----- time constraints ------------ %

constraint 
   forall(n in CUSTOMER) (
      arrivalTime[n] + distance[n,successor[n]] <= arrivalTime[successor[n]]
   );
constraint
   forall(n in START_DEPOT_NODES) (
      arrivalTime[n] + distance[n,successor[n]] <= arrivalTime[successor[n]]
   );

% ----- load constraints ------------ %

constraint 
   forall(n in CUSTOMER) (
      load[n] + demand[n] = load[successor[n]]
   );
constraint
   forall(n in START_DEPOT_NODES) (
      load[n] = load[successor[n]] 
   );


% =====================================
% Objective & Search
% =====================================

int: iterations = 10; 
int: initRate = 30;
int: timeout_lns_ms = 10*1000;  % timeout for all LNS iterations
int: timeout_bab_ms = 3*000;  % timeout for one BAB execution
bool: verbose = true;

constraint
 traveltime = sum (depot in END_DEPOT_NODES) (arrivalTime[depot]);

solve :: seq_search([int_search([successor[j] | j in NODES], first_fail, indomain_split, complete),
               int_search(vehicle, first_fail, indomain_split, complete),
               int_search([arrivalTime[j] | j in NODES],first_fail, indomain_min, complete),
               int_search([load[j] | j in NODES], first_fail, indomain_min, complete)
              ])   
% LNS with restricted number of iterations and an overall time limit
search 
     time_limit(timeout_lns_ms, 
                adaptive_lns(traveltime, successor, iterations, initRate, timeout_bab_ms)) /\
     print("test\n") /\
     if hasSol() then print() 
                 else print("No solution found within time limit.\n") endif
;


function ann: adaptive_lns(var int: obj, array[int] of var int: vars,
                           int: iterations, int: initRate, int: exploreTime) =
  let { int: nSize = initRate, int: step = 1; } in
  repeat (i in 1..iterations) (
    scope(
         if hasSol() then 
           print("Iteration "++show(i)++", rate="++show(nSize)++"\n") /\ 
          ( post(uniformNeighbourhood1(vars,nSize/100.0)) /\
             time_limit(exploreTime, bab_min(obj)) /\
             commit() /\ print()
           ) \/ (nSize := nSize + step /\ fail)  
         else next() /\ print() /\ commit() endif
   )
   /\ post(obj < sol(obj))   
);


predicate uniformNeighbourhood1(array[int] of var int: x, float: destrRate) = 
  if hasSol() then
    forall(i in index_set(x)) (
      if uniform(0.0,1.0) > destrRate then x[i] = sol(x[i]) 
         %/\ trace("x["++show(i)++"] = "++show(sol(x[i]))++";\n") 
      else true endif )
  else true endif;

% ===================================== %
% Instance P-n16-k8.vrp.dzn
% ===================================== %


N = 15;
Capacity = 35;
Demand = [19, 30, 16, 23, 11, 31, 15, 28, 8, 8, 7, 14, 6, 19, 11];
Distance = [| 0, 14, 21, 33, 22, 23, 12, 22, 32, 32, 21, 28, 30, 29, 31, 30
| 14, 0, 12, 19, 12, 24, 12, 19, 21, 27, 7, 19, 16, 21, 33, 17
| 21, 12, 0, 15, 22, 16, 11, 9, 12, 15, 11, 29, 19, 9, 24, 23
| 33, 19, 15, 0, 21, 31, 25, 23, 8, 24, 12, 25, 9, 17, 37, 16
| 22, 12, 22, 21, 0, 36, 24, 30, 26, 37, 12, 7, 13, 30, 44, 9
| 23, 24, 16, 31, 36, 0, 13, 8, 25, 13, 26, 43, 35, 16, 8, 39
| 12, 12, 11, 25, 24, 13, 0, 10, 23, 20, 16, 31, 26, 17, 21, 28
| 22, 19, 9, 23, 30, 8, 10, 0, 18, 10, 19, 37, 28, 9, 15, 32
| 32, 21, 12, 8, 26, 25, 23, 18, 0, 17, 15, 32, 17, 10, 31, 23
| 32, 27, 15, 24, 37, 13, 20, 10, 17, 0, 25, 44, 31, 7, 16, 37
| 21, 7, 11, 12, 12, 26, 16, 19, 15, 25, 0, 19, 10, 18, 34, 13
| 28, 19, 29, 25, 7, 43, 31, 37, 32, 44, 19, 0, 16, 37, 51, 10
| 30, 16, 19, 9, 13, 35, 26, 28, 17, 31, 10, 16, 0, 24, 43, 6
| 29, 21, 9, 17, 30, 16, 17, 9, 10, 7, 18, 37, 24, 0, 21, 30
| 31, 33, 24, 37, 44, 8, 21, 15, 31, 16, 34, 51, 43, 21, 0, 47
| 30, 17, 23, 16, 9, 39, 28, 32, 23, 37, 13, 10, 6, 30, 47, 0
|];



% ===================================== %
% Output
% ===================================== %

output 
   [ "traveltime = "] ++ [show(traveltime)] ++
   [ "\nvehicle = " ] ++ 
   [ show(vehicle[c]) ++ ","| c in 1..nbCustomers ]++
   [ "\narrival-time at depot = " ] ++ 
   [ show(arrivalTime[n]) ++ ","| n in END_DEPOT_NODES ]++
   [ "\nsuccessor = "] ++
          [ show(successor[n]) ++ ", "
            | n in NODES ] ++
   [ "\n", show(traveltime)]; % for script

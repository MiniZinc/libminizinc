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
% Objective
% =====================================

int: iterations = 10; % adjust this number depending on problem instance size?
float: destroyrate = 0.3;
int: timeout_lns_ms = 30*60*1000;  % timeout for all LNS iterations
int: timeout_bab_ms = 3000;  % timeout for one BAB execution
bool: verbose = true;

constraint
 traveltime = sum (depot in END_DEPOT_NODES) (arrivalTime[depot]);

solve :: seq_search([int_search([successor[j] | j in NODES], first_fail, indomain_split, complete),
               int_search(vehicle, first_fail, indomain_split, complete),
               int_search([arrivalTime[j] | j in NODES],first_fail, indomain_min, complete),
               int_search([load[j] | j in NODES], first_fail, indomain_min, complete)
              ])   
% LNS with restricted number of iterations and an overall time limit
:: 
combinator(
     lns_min(traveltime, successor, iterations, destroyrate,verbose) 
)
satisfy
;



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

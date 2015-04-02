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

constraint
 traveltime = sum (depot in END_DEPOT_NODES) (arrivalTime[depot]);



% =====================================
% Search
% =====================================
int: iterations = 10;
int: subIterations = 4;
float: destroyrate = 0.6;
float: secs = 5.0;

solve 
:: seq_search([int_search([successor[j] | j in NODES], first_fail, indomain_split, complete),
               int_search(vehicle, first_fail, indomain_split, complete),
               int_search([arrivalTime[j] | j in NODES],first_fail, indomain_min, complete),
               int_search([load[j] | j in NODES], first_fail, indomain_min, complete)
              ])   
:: combinator(

  or([
    repeat (i in 1..iterations) (
      and([
        scope(
          or([
            and([
              post(neighbourhood()),
              next(time_limit(secs)),
              print(),
            ]),
          print()
          ])
        ),
       post(traveltime < sol(traveltime)),
       print()
      ])
    ),
    print()
   ])
  )

satisfy;


predicate neighbourhood() = 
  let {  
    % flip a coin on which neighbourhood to use
    float: coin = uniform(0.0,1.0);
  } in
   if coin > 0.5 then     
     keepRouteWithMostCustomers()
   else 
     keepRandomRoute()
   endif
  /\
   if coin < 0.2 then
     useRandomNewVehicle()
   else true
   endif  
;


predicate useRandomNewVehicle() =
  if(hasSol()) then 
  let {
    set of int: unusedVehicles = { v | v in 1..nbVehicles where (sol(successor[nbCustomers+v]) == nbCustomers+nbVehicles+v) };
    array[1..nbVehicles] of int: distribution = [ (v in unusedVehicles)  | v in 0..nbVehicles ];
    int: vehicleId = discrete_distribution(distribution);
  } in 
    if card(unusedVehicles) > 0 then 
      % the vehicle is not allowed to stay in the depot
      trace("successor["++show(nbCustomers+vehicleId)++"] < "++show(nbCustomers+nbVehicles+vehicleId)++"\n") /\
      successor[nbCustomers+vehicleId] < nbCustomers+nbVehicles+vehicleId
    else true
    endif
 else true 
 endif
;


predicate keepRouteWithMostCustomers() = 
if(hasSol()) then
  let {
    array[1..nbVehicles] of int: nbServedCustomers = [ sum(c in 1..nbCustomers) (sol(vehicle[c]) == v) | v in 1..nbVehicles ];
    int: maxServedCustomers = max(nbServedCustomers);
  } in 
   forall (v in 1..nbVehicles) (
     if nbServedCustomers[v] == maxServedCustomers then 
       trace("keeping route with most customers: vehicle "++show(v)++"\n") /\
       keepRoute(v)
     else true
     endif
   )
else true
endif
;

predicate keepVehicleWithMostCustomers() = 
if(hasSol()) then
  let {
    array[1..nbVehicles] of int: nbServedCustomers = [ sum(c in 1..nbCustomers) (sol(vehicle[c]) == v) | v in 1..nbVehicles ];
    int: maxServedCustomers = max(nbServedCustomers);
  } in 
   forall (v in 1..nbVehicles) (
     if nbServedCustomers[v] == maxServedCustomers then 
       trace("keeping vehicle with most customers: vehicle "++show(v)++"\n") /\
       keepVehicle(v)
     else true
     endif
   )
else true
endif
;



predicate keepRandomRoute() = 
  if(hasSol()) then 
    let {
       set of int: usedVehicles = { v | v in 1..nbVehicles where exists(c in 1..nbCustomers) (sol(vehicle[c]) == v) };
       array[1..nbVehicles] of int: distribution = [ (v in usedVehicles)  | v in 0..nbVehicles ];
       int: v = discrete_distribution(distribution);
    } in
     trace("keeping random vehicle "++show(v)++"\n") /\
     keepRoute(v)
  else 
    true
  endif
;


predicate keepRoute(int: vNum) =
if hasSol() then 
 forall (i in 1..nbCustomers) (
   if sol(vehicle[i]) == vNum then 
     trace("posting: successor["++show(i)++"] = "++show(sol(successor[i]))++"\n") /\
     successor[i] = sol(successor[i])
   else true
   endif
 )
else true
endif
;


predicate keepVehicle(int: vNum) =
if hasSol() then 
 forall (i in 1..nbCustomers) (
   if sol(vehicle[i]) == vNum then 
     trace("posting: vehicle["++show(i)++"] = "++show(vNum)++"\n") /\
     vehicle[i] = vNum
   else true
   endif
 )
else true
endif
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

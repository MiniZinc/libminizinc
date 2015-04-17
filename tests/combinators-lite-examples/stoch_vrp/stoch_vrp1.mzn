%-----------------------------------------------------------------------------%
% Vehicle Routing problem with stochastic travel times
% stochastic model 
%
% First stage decisions: vehicle <-> customer assignment
% Second stage decisions: tour of each vehicle 
% We assume that there is only one depot
%
% modelled by Andrea Rendl, 2014.
%-----------------------------------------------------------------------------%
include "combinators.mzn";
include "globals.mzn";

int: nbVehicles;
int: nbCustomers;
int: timeBudget; % the maximal of time that we got

set of int: VEHICLE = 1..nbVehicles;
set of int: CUSTOMER = 1..nbCustomers;
set of int: TIME = 0..timeBudget;

% the last nodes represent the start and end node for each vehicle (the depot)
set of int: NODES = 1..nbCustomers+2*nbVehicles; 
set of int: DEPOT_NODES = nbCustomers+1..nbCustomers+2*nbVehicles;
set of int: START_DEPOT_NODES = nbCustomers+1..nbCustomers+nbVehicles;
set of int: END_DEPOT_NODES = nbCustomers+nbVehicles+1..nbCustomers+2*nbVehicles;

array[NODES] of int: serviceTime; % service time at each node

int: nbScenarios;
set of int: SCENARIOS = 1..nbScenarios;
array[SCENARIOS] of int: weights; % weights of each scenario
array[SCENARIOS, NODES, NODES] of int: distance;  % distance matrix is stochastic

% -------- variables ------------- %

array[NODES] of var VEHICLE: vehicle; %::stage(1); % which vehicle visits which customer?

% -------- constraints ---------- %

% ...... initialization constraints ---- %

% associate each start/end nodes with a vehicle
constraint 
   forall(n in START_DEPOT_NODES) (
     vehicle[n] = n-nbCustomers
   );
   
constraint 
   forall(n in END_DEPOT_NODES) (
     vehicle[n] = n-nbCustomers-nbVehicles
   );


solve ::int_search(vehicle, first_fail, indomain_split, complete) 
search policy_based_search_min();


% ================= SEARCH ================================= %

int: iterations = 10;

function ann: policy_based_search_min() = 
   let {% initialize the objective values of each scenario with zero
     array[SCENARIOS] of int: scenario_obj = [ 0 | i in SCENARIOS ]; 
     int: expectedObj = 1000000;
   } 
   in  (
         repeat (i in 1..iterations) (      
               (next() \/ break)  /\ % find a solution for the first stage variables
               print("First stage solution found\n") /\ print() /\
               repeat (s in 1..nbScenarios) (
                  scope(
                     let { 
                         % second stage variables
                         array[NODES] of var NODES: successor; 
                         array[NODES] of var NODES: predecessor; 
                         array[NODES] of var TIME: arrivalTime;
                         var int: obj; % the second stage objective
                     } in (                                        
                         post(firstStageAssignment() /\ % assign first stage variables
                              secondStageConstraints(s,obj,successor,predecessor,arrivalTime)) /\
                         if bab_min(obj) then
                           print("Objective for scenario "++show(s)++": "++show(sol(obj))++"\n") /\
                           scenario_obj[s]  := sol(obj)
                         else print("No second stage solution found for scenario "++show(s)++"\n") /\
                              break 
                         endif                      
                     )
                 )
              ) % end repeat 
              /\ if expectedObj > sum (i in SCENARIOS) (weights[i]*(scenario_obj[i])) then 
                    expectedObj := sum (i in SCENARIOS) (weights[i]*(scenario_obj[i]))
                     /\ print("Best expected objective:"++show(expectedObj)++"\n") /\ print() % we found a solution
                  else %print("So-far best expected profit: "++show(expectedProfit)++" is better than just found expected profit: "++show(sum (i in SCENARIOS) (weights[i]*(scenario_obj[i])))++"\n") /\
                       skip() endif
        )
    )
;

% ======== output ============= %

predicate firstStageAssignment() = 
  forall (i in VEHICLE)  (
    vehicle[i] = sol(vehicle[i])
    %/\  trace("vehicle["++show(i)++"] = "++show(fix(vehicle[i]))++" (sol)\n")
  )
;

predicate secondStageConstraints(int: scenario, 
                                 var int: obj, 
                                 array[NODES] of var int: successor,
                                 array[NODES] of var int: predecessor,
                                 array[NODES] of var int: arrivalTime) = 
% ...... initialisation constraints ......... %

% predecessor of start nodes are end nodes
   forall(n in (nbCustomers+2..nbCustomers+nbVehicles)) (
     predecessor[n] = n + nbVehicles-1
   )
/\
   predecessor[nbCustomers+1] = nbCustomers+2*nbVehicles

/\  % successors of end nodes are start nodes
   forall(n in (nbCustomers+nbVehicles+1..nbCustomers+2*nbVehicles-1)) (
     successor[n] = n-nbVehicles+1 
   )
/\
   successor[nbCustomers+2*nbVehicles] = nbCustomers+1
/\
% vehicles leave the depot at time zero
   forall(n in START_DEPOT_NODES) (
     arrivalTime[n] = 0 
   )
/\

% vehicle of node i is the same as the vehicle for the predecessor
   forall(n in CUSTOMER) (
      vehicle[predecessor[n]] = vehicle[n]
   )
/\
   forall(n in CUSTOMER) (
      vehicle[successor[n]] = vehicle[n]
   )
/\

% ...... predecessor/successor constraints ... %
   forall(n in NODES) (
      successor[predecessor[n]] = n
   )
/\
   forall(n in NODES) (
      predecessor[successor[n]] = n
   )
/\
% alldiff + subtour elimination constraints
   circuit( [ successor[i] | i in NODES ] )
/\
   circuit( [ predecessor[i] | i in NODES ] )


% ..... time constraints ............ %
/\
   forall(n in CUSTOMER) (
      arrivalTime[n] + serviceTime[n] + distance[scenario,n,successor[n]] <= arrivalTime[successor[n]]
   )
/\
   forall(n in START_DEPOT_NODES) (
      arrivalTime[n] + serviceTime[n] + distance[scenario,n,successor[n]] <= arrivalTime[successor[n]]
   )
/\

% ..... objective ....... %
obj = max( [ arrivalTime[i] | i in NODES ])
;



output 
   [ "vehicle = " ] ++ 
   [ show(vehicle[c]) ++ ","| c in NODES ];
   
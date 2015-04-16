% ==============================================================
% Simple transportation/production problem
% two-stage recourse model 
% 
% modelled by Andrea Rendl, 2013.
% ==============================================================
include "combinators.mzn";

% ------------ CONSTANTS ---------------------------------------

set of int: FACTORY = 1..2; int: melbourne = 1; int: sydney = 2;
set of int: CUSTOMER = 1..5; int: adelaide = 1;  int: brisbane = 2; 
int: canberra = 3;  int: darwin = 4; int: perth = 5;

int: maxTransport = 1000;
int: maxProduction = 1000;
int: maxPenaltyCost = 1000;
int: maxStorageCost = 1000;
int: maxCosts = 99000;

int: storageCost = 1;  % cost for storing 1 unit at the factory
int: unmetDemandPenalty = 100; % penalty for not meeting 1 unit of demand

% ------------ PARAMETERS --------------------------------------

% the transportation costs per transported unit
array[FACTORY,CUSTOMER] of int: transportCosts;

array[FACTORY] of int: productionCost;

int: nbScenarios;
set of int: SCENARIOS = 1..nbScenarios;
array[SCENARIOS] of int: weights; % weights on each scenario
% the demand of each customer
array[SCENARIOS,CUSTOMER] of int: demand; % demand is stochastic

% ------------ VARIABLES --------------------------------------

% first stage variables
array[FACTORY] of var 0..maxProduction: production;
var int: firstStageCosts;

% ------------ CONSTRAINTS ------------------------------------

constraint 
  firstStageCosts =   (sum (f in FACTORY) (productionCost[f]*production[f]));

% ------------ SEARCH --------------------------------------


%solve minimize
%    firstStageCosts +
%     overallTransportCosts +
%      sum (c in CUSTOMER) (demandPenaltyCosts[c]) +
%      sum (f in FACTORY) (storageCosts[f]) 
%   ) :: expected
%   ;

int: iterations = 10;

solve search 
  policy_based_search_min();


function ann: policy_based_search_min() = 
   let { % initialize the objective values of each scenario with zero
     array[SCENARIOS] of int: scenario_obj = [ 0 | i in SCENARIOS ]; 
     int: expectedCosts = infinity; %incumbent objective
   } 
   in  (
         repeat ( %(i in 1..iterations) (      
            if next() then  % find a solution for the first stage variables
               %print("First stage solution found\n") /\
               repeat (s in 1..nbScenarios) (
                  scope(
                     let { 
                         % second stage variables (recourse)
                         array[FACTORY] of var 0..maxStorageCost: storageCosts;
                         array[FACTORY,CUSTOMER] of var 0..maxTransport: transport;
                         array[CUSTOMER] of var 0..maxPenaltyCost: demandPenaltyCosts;
                         var 0..maxCosts: overallTransportCosts;
                         var 0..maxCosts: scenarioCosts; % objective for scenario
                     } in (                                        
                         post(firstStageAssignment() /\ % assign first stage variables
                              secondStageConstraints(s,scenarioCosts,transport,storageCosts,demandPenaltyCosts,overallTransportCosts)) /\
                         if bab_max(scenarioCosts) then
                           %print("Costs for scenario "++show(s)++": "++show(sol(scenarioCosts))++"\n") /\
                           scenario_obj[s]  := sol(scenarioCosts)
                         else %print("No second stage solution found for scenario "++show(s)++"\n") /\
                              break 
                         endif                      
                     )
                 )
              ) % end repeat 
              /\ if expectedCosts > expectedValue(scenario_obj) then 
                    expectedCosts := expectedValue(scenario_obj)
                    /\ print("Best expected costs:"++show(expectedCosts)++"\n") /\ print() % we found a solution
                 else  skip() endif
           else break endif % we find no first stage solution 
        )
    )
;


function int: expectedValue(array[SCENARIOS] of int: scenario_obj) =
   % minimize the the sum over all costs
   sol(firstStageCosts) + sum (i in SCENARIOS) (weights[i]*(scenario_obj[i]));


predicate firstStageAssignment() = 
  forall (i in FACTORY)  (
     production[i] = sol(production[i])
  )
;

predicate secondStageConstraints(int: scenario, 
                                 var int: obj,
                                 array[FACTORY,CUSTOMER] of var 0..maxTransport: transport,
                                 array[FACTORY] of var 0..maxStorageCost: storageCosts,
                                 array[CUSTOMER] of var 0..maxPenaltyCost: demandPenaltyCosts,
                                 var 0..maxCosts: overallTransportCosts) = 
% the factories cannot transport more than they supply/produce
   forall (f in FACTORY) (
       sum (c in CUSTOMER) (transport[f,c]) <= production[f]
   )
/\

% transportation costs
   overallTransportCosts = sum (f in FACTORY, c in CUSTOMER) 
		                 (transport[f,c]*transportCosts[f,c])
/\

% we need to pay a penalty for each unit of unmet demand
   forall (c in CUSTOMER) (
     demandPenaltyCosts[c] >= unmetDemandPenalty*
	             (demand[scenario,c] - sum (f in FACTORY) (transport[f,c]))	       
   )

/\

% everything that we do not ship/transport needs to be stored
   forall (f in FACTORY) (
       storageCosts[f] >= storageCost*
		         (production[f] - sum(c in CUSTOMER) (transport[f,c]))
   )

/\

obj =  overallTransportCosts +
      sum (c in CUSTOMER) (demandPenaltyCosts[c]) +
      sum (f in FACTORY) (storageCosts[f]) 
;

% ------------ OUTPUT --------------------------------------

% output the first stage decisions
output [ " production = \n"] ++
       [ show(production[f]) ++
         " "
       | f in FACTORY];
% --------------------------------------------------------------- % 
%  Stochastic knapsack problem  
% 
%  stochastic parameter: The profit of each item
%  first stage decision: Selection of items
%
%  Nitika Mathur, 2014.
% --------------------------------------------------------------- % 


include "globals.mzn";
include "combinators.mzn";

% Parameters
int: nItems;
set of int: ITEMS = 1..nItems;

array[ITEMS] of int: size;
int: capacity;
int: minProfit;
array[SCENARIOS,ITEMS] of int: profit; % :: stage(2): the profit is uncertain  
int: nbScenarios;
set of int: SCENARIOS = 1..nbScenarios;
array[SCENARIOS] of int: weights;

%Decision Variable 
array[ITEMS] of var 0..capacity: selectedItems; % :: stage(1) 
int: maxProfit = 9999999;
%array[SCENARIOS] of var 0..maxProfit: totalProfit; % ::stage(2)
var 0..maxProfit: expectedProfit;

constraint forall(i in ITEMS) (selectedItems[i] >= 0);
 
%Capacity constraint
constraint 
	sum (i in ITEMS) (size[i] * selectedItems[i]) <= capacity;

 
% Solve
solve 
:: combinator(
   let {  % issue: this does not have to start a new scope, since we are just using it to define a constant array
     array[SCENARIOS] of int: scenario_obj = [ 0 | i in SCENARIOS ]; % initialize the objective values of each scenario with zero
   } 
   in  (
         repeat (
           and([
               next(), % find a solution for the first stage variables
               repeat (s in 1..nbScenarios) (
                 let { 
                    var 0..maxProfit: totalProfit; % second stage variables
                    % what if variable-id already exists?
                    % current issue: no solution found for first stage vars -> part of the solution is already set when applying next solution to output model 
                 } in (                 
                   and([
                     post(firstStageAssignment()), % assign first stage variables
                     post(secondStageConstraints(s,totalProfit)),
                     bab_max(totalProfit),
                     scenario_obj[s]  := sol(totalProfit)
%                     comb_assign(scenario_obj[s],sol(totalProfit)) % issue: this cannot be written as e1 := e2 (for type reasons, I think)
                   ]) 
                 )
              ),
              post(expectedProfit > sum (i in SCENARIOS) (weights[i]*sol(scenario_obj[i]))) % BAB constraint on overall obj var
           ])
       )
  )
) satisfy;

predicate secondStageConstraints(int: scenario, var int: p) = 
  p = sum(i in ITEMS)(profit[scenario,i] * selectedItems[i]) /\
  p >= minProfit
;

predicate firstStageAssignment() = 
  forall (i in ITEMS)  (
     selectedItems[i] = sol(selectedItems[i])
  )
;
     


output ["expected profit = "++show(expectedProfit)++"\n",        
        "selectedItems = ", show(selectedItems), "\n" ];
%        "total profit per scenario = ", show(totalProfit), "\n"       ];





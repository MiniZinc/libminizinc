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


constraint forall(i in ITEMS) (selectedItems[i] >= 0);
 
%Capacity constraint
constraint 
	sum (i in ITEMS) (size[i] * selectedItems[i]) <= capacity;

int: iterations = 10000;
 
% Solve
solve :: int_search(selectedItems,first_fail,indomain_max,complete)
search policy_based_search();


function ann: policy_based_search() = 
   let {  
     array[SCENARIOS] of int: scenario_profit = [ 0 | i in SCENARIOS ]; % initialize the objective values of each scenario with zero
     int: expectedProfit = 0;
   } 
   in  (
         repeat (i in 1..iterations) (      
               (next() \/ break)  /\ % find a solution for the first stage variables
               repeat (s in 1..nbScenarios) ( % make sure the first stage solution is valid for each scenario
                 if sum(i in ITEMS)(profit[s,i] * sol(selectedItems[i])) > minProfit then 
                     scenario_profit[s]  := sum(i in ITEMS)(profit[s,i] * sol(selectedItems[i]))
                     %/\ print("First stage solution is valid for scenario "++show(s)++"\n")
                 else break 
                 endif                      
               )
              /\ if expectedProfit < sum (i in SCENARIOS) (weights[i]*(scenario_profit[i])) div sum (i in 1..nbScenarios) (weights[i]) then 
                    expectedProfit := sum (i in SCENARIOS) (weights[i]*(scenario_profit[i])) div sum (i in 1..nbScenarios) (weights[i])
                     /\ commit() /\ print("Best expected profit:"++show(expectedProfit)++"\n") /\ print() % we found a solution
                  else skip() endif
        )
    )
;

output [ "selectedItems = ", show(selectedItems), "\n" ];





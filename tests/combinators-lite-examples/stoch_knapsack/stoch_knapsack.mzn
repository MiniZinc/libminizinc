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
solve :: int_search(selectedItems,first_fail,indomain_max,complete)
:: combinator(
   let {  
     array[SCENARIOS] of int: scenario_obj = [ 0 | i in SCENARIOS ]; % initialize the objective values of each scenario with zero
   } 
   in  (
         repeat (
           and([
               next(), % find a solution for the first stage variables
               print(),
               or([
                 repeat (s in 1..nbScenarios) (
                     let { 
                        var minProfit..maxProfit: totalProfit; % second stage variables
                     } in (                 
                       and([
                         post(firstStageAssignment() /\ % assign first stage variables
                              secondStageConstraints(s,totalProfit)),                     
                         bab_max(totalProfit),
                         print(),
                         scenario_obj[s]  := sol(totalProfit)
                       ]) 
                     )
                   ), % end repeat
                prune]),
                % BAB constraint on overall obj var
              post(expectedProfit > sum (i in SCENARIOS) (weights[i]*sol(scenario_obj[i]))) 
           ])
       )
  )
) satisfy;

predicate secondStageConstraints(int: scenario, var int: p) = 
  p = sum(i in ITEMS)(profit[scenario,i] * selectedItems[i]) /\
  trace("profit["++show(scenario)++"] = ") /\
  forall (i in ITEMS) (trace(show(profit[scenario,i])++"*"++show(selectedItems[i])++" + ")) /\
  trace(" == "++show(sum(i in ITEMS) (profit[scenario,i] * fix(selectedItems[i])))) /\
  trace("\n")
;

predicate firstStageAssignment() = 
  forall (i in ITEMS)  (
     selectedItems[i] = sol(selectedItems[i]) /\
     trace("selectedItems["++show(i)++"] = "++show(fix(selectedItems[i]))++" (sol)\n")
  )
;
     


output ["expected profit = "++show(expectedProfit)++"\n",        
        "selectedItems = ", show(selectedItems), "\n" ];
%        "total profit per scenario = ", show(totalProfit), "\n"       ];





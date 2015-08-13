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



% ----------------------------------------------------------------------
% Instance 
% ----------------------------------------------------------------------

nItems = 10;
capacity = 10000; %2426;
minProfit = 2426;
size =  [107, 119, 188, 265, 374, 481, 751, 797, 811, 960];

nbScenarios = 2;
weights =[3, 2];
profit = array2d(1..2, ITEMS, 
[
 	%Scenario 1
	81, 69, 276, 291, 422, 572, 886, 741, 405, 1363,
	%Scenario 2
	115, 132, 236, 233, 340, 399, 660, 884, 1240, 912,
] ); 

% ----------------------------------------------------------------------
% Objective and Search
% ----------------------------------------------------------------------

int: iterations = 10;
 
% Solve
solve :: int_search(selectedItems,first_fail,indomain_max,complete)
search 
%my_search(iterations);



%function ann: my_search(int: iterations) =
   let {  
     array[SCENARIOS] of int: scenario_obj = [ 0 | i in SCENARIOS ]; % initialize the objective values of each scenario with zero
     int: expectedProfit = 0;
   } 
   in  (
         repeat (i in 1..iterations) (      
               (next() \/ break)  /\ % find a solution for the first stage variables
               %print("First stage solution found\n") /\
               repeat (s in 1..nbScenarios) (
                  scope(
                     let { 
                        var minProfit..maxProfit: totalProfit; % second stage variables
                     } in (                                        
                         post(firstStageAssignment() /\ % assign first stage variables
                              totalProfit = sum(i in ITEMS)(profit[s,i] * selectedItems[i])) /\
%                              secondStageConstraints(s,totalProfit)) /\
                         if bab_max(totalProfit) then
                           print("Maximal total profit for scenario "++show(s)++": "++show(sol(totalProfit))++"\n") /\
                           scenario_obj[s]  := sol(totalProfit)
                         else print("No second stage solution found for scenario "++show(s)++"\n") /\
                              break 
                         endif                      
                     )
                 )
              ) % end repeat 
              /\ if expectedProfit < sum (i in SCENARIOS) (weights[i]*(scenario_obj[i])) then 
                    expectedProfit := sum (i in SCENARIOS) (weights[i]*(scenario_obj[i]))
                     /\ print("Best expected profit:"++show(expectedProfit div sum(i in SCENARIOS)(weights[i]))++"\n") /\ print() % we found a solution
   else %print("So-far best expected profit: "++show(expectedProfit)++" is better than just found expected profit: "++show(sum (i in SCENARIOS) (weights[i]*(scenario_obj[i])))++"\n") /\
                       skip() endif
        )
    )
;

predicate secondStageConstraints(int: scenario, var int: p) = 
  p = sum(i in ITEMS)(profit[scenario,i] * selectedItems[i]) %/\
%  trace("profit["++show(scenario)++"] = ") /\
%  forall (i in ITEMS) (trace(show(profit[scenario,i])++"*"++show(selectedItems[i])++" + ")) /\
%  trace(" == "++show(sum(i in ITEMS) (profit[scenario,i] * fix(selectedItems[i])))) /\
%  trace("\n")
;

predicate firstStageAssignment() = 
  forall (i in ITEMS)  (
     selectedItems[i] = sol(selectedItems[i]) %/\
     %trace("selectedItems["++show(i)++"] = "++show(sol(selectedItems[i]))++" (sol)\n")
  )
;
     


output [%"expected profit = "++show(expectedProfit)++"\n",
        "selectedItems = ", show(selectedItems), "\n "]
%        "totalProfit = " ++ show(totalProfit) ++ "\n"];
%        "total profit per scenario = ", show(totalProfit), "\n"       ];





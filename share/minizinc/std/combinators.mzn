%%%  SEARCH COMBINATORS LITE

% =================================================================== %
%                   combinator lite built-ins                         %
% =================================================================== %

% execute the given annotations until one fails
annotation and(array[int] of ann);
% dummy annotation to wrap combinator in to avoid flattening of combinator
annotation combinator (ann);
% conditional: if c is successful then comb1 else comb2
annotation cond (ann: c, ann: comb1, ann: comb2);
% return UNSAT
annotation fail;
% limit the number of fails during search in the execution of next
annotation fail_limit(int: fails);
% direct the solver to find next solution, returns true if found and
% false otherwise
annotation next;
% next can have one argument (e.g. time limits)
annotation next(ann);
% next can have several arguments (e.g. time limit and node limit)
annotation next(array [int] of ann);
% limit the search nodes in the execution of next
annotation node_limit(int: nodes);
% execute the given annotations until all fail
annotation or(array[int] of ann);
% post constraint expression expr in current scope
annotation post(var bool: expr); 
% print the last found solution according to the output statement
annotation print;
% return UNKNOWN
annotation prune;
% repeat annotations for a limited number of times
annotation repeat(array[int] of ann);
% repeat annotations an unlimited number of times until the annotation
% is done or fails
annotation repeat(ann);
% define an new scope 
annotation scope(ann);
% do nothing and return SAT (mainly for use in if-statements)
annotation skip;
% limit execution time of next to the given number of milliseconds
annotation time_limit(int: ms);
% limit execution time of next to the given number of milliseconds
annotation time_limit(int: ms, ann);

% print the output as print()
function ann: print; 
% get the next solution as next()
function ann: next; 
% do nothing: return SAT, skip()
function ann: skip;
% return UNSAT
function ann: fail;
% return UNKNOWN
function ann: prune;

% assign search variable x to y
annotation comb_assign(int: x,int: y);

% assign solution variable s to result of c
annotation comb_assign(ann: s,ann: c);

% commit solution s to current function scope
annotation commit(ann: s);

annotation empty_model;

% =================================================================== %
%                    the actual combinators                           %
% =================================================================== %

% Branch&Bound minimize combinator
include "bab_min.mzn";

% Branch and Bound when maximising
include "bab_max.mzn";

%% ------------------- Large Neighbourhood Search -------------------- %
%% Large Neighbourhood Search variant that takes an objective variable
%% and an array of search variables, a number of iterations and a 
%% destruction rate d (that determines the percentage of variables in x 
%% that will be destroyed, i.e. the size of the neighbourhood to search)
%function ann: lns_min (var int: obj, array[int] of var int: x,
%                   int: iterations, float: d) = 
%    repeat (i in 1..iterations) (
%      and([
%        scope(
%          and([ 
%            post(neighbourhoodCts(x,d)),
%            or([bab_min(obj,print),print()]) %next, % or bab
%          ]),
%        ),
%        post(obj < sol(obj))
%      ])
%  );
%
%function ann: lns_min (var int: obj, array[int] of var int: x,
%                   int: iterations, float: d, int: timeout_ms) = 
%    repeat (i in 1..iterations) (
%      and([
%        scope(
%          or([
%            and([ 
%              post(neighbourhoodCts(x,d)),
%              time_limit(timeout_ms,
%                         bab_min(obj) %next
%              )
%            ]),
%            print()
%          ])
%        ),
%        post(obj < sol(obj))
%      ])
%  );
%
%% Predicate that randomly choses the neighbourhood to keep, and then 
%% posts the respective assignment constraints (assigning those variables 
%% to the value in the last solution)
%predicate neighbourhoodCts(array[int] of var int: x, 
%                            float: destructionRate) = 
%   if(hasSol()) then 
%     let {
%       int: n = ceil((1.0 - destructionRate)*length(x));
%       array[1..n] of int: idx = [ uniform(1,length(x)) | i in 1..n ];
%     } in 
%       forall (i in idx) (
%         %trace("x["++show(i)++"] = "++show(sol(x[i]))++"\n") /\
%         x[i] = sol(x[i])
%       )     
%   else true endif;
%
%
%% ------------------- Two Stage Policy Based Search ------------------ %
%function ann: policy_based_two_stage(array[int] of var int: stage1,
%                                     array[int] of var int: stage2,
%                                     var int: obj,
%                                     int: nbScenarios) = 
%
%   let { int: overall_obj = 0, % overall objective value
%         array[1..nbScenarios] of int: scenario_obj = 
%                                        [ 0 | i in 1..nbScenarios ]  } 
%   in  
%     and([
%         repeat (
%           and([
%             next, % TODO: do we want the variable assignments to be fixed according to this solution in all subscopes?
%               scope(
%                 repeat (s in 1..nbScenarios) (
%                   and([                     
%                     post(secondStageConstraints(s)),
%                     let {
%                       var int: obj_s = getObjective(s);          
%                     } in
%                       and([bab_min(obj_s),
%                           % scenario_obj[s] := val(obj) % does not work
%                           comb_assign(scenario_obj[s], sol(obj_s))])
%                   ]) 
%                 )
%              )
%           ])
%         ), 
%       % overall_obj := sum (i in 1..nbScenarios) scenario_obj[i])
%         comb_assign(overall_obj,
%                   sum (i in 1..nbScenarios) (scenario_obj[i])),
%         post(obj < overall_obj) % BAB constraint on overall obj var
%     ])
%;
%
%% returns the objective variable for the given scenario 
%function var int: getObjective (int: scenario);
%
%% define this predicate in your model, defining the second stage 
%% constraints for the given scenario
%predicate secondStageConstraints(int: scenario); 

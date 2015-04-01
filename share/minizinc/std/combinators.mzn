%%%  SEARCH COMBINATORS LITE

% =================================================================== %
%                   combinator lite built-ins                         %
% =================================================================== %

% dummy annotation to wrap combinator in to avoid flattening of combinator
annotation combinator (ann);
% direct the solver to find next solution, returns true if found and
% false otherwise
annotation next;
% next can have one argument (e.g. time limits)
annotation next(ann);
% next can have several arguments (e.g. time limit and node limit)
annotation next(array [int] of ann);
% post constraint expression expr in current scope
annotation post(var bool: expr); 
% print the last found solution according to the output statement
annotation print;
% define an new scope 
annotation scope(ann);
% repeat annotations for a limited number of times
annotation repeat(array[int] of ann);
% repeat annotations an unlimited number of times until the annotation
% is done or fails
annotation repeat(ann);
% execute the given annotations until one fails
annotation and(array[int] of ann);
% execute the given annotations until all fail
annotation or(array[int] of ann);
% limit execution time of next to the given number of seconds
annotation time_limit(int: secs);
annotation time_limit(float: secs);
% limit the search nodes in the execution of next
annotation node_limit(int: nodes);
% limit the number of fails during search in the execution of next
annotation fail_limit(int: fails);

% return the solution value of the last solution
%function int: sol(var int: x); 
%function bool: sol(var bool: x);
%function float: sol(var float: x);
% returns true if there already exists a (last) solution, and returns
% false otherwise
%function bool: hasSol; 
% print the output as print()
function ann: print; 
% get the next solution as next()
function ann: next; 

% assign search variable x to y
annotation comb_assign(int: x,int: y);

% =================================================================== %
%                    the actual combinators                           %
% =================================================================== %

% ------------------- Branch & Bound -------------------------------- %
% Branch and Bound when minimizing
function ann: bab_min(var int: obj, ann: s) =   
       repeat(
           and([
               next, % get next solution
               post(obj < sol(obj)), % post constraint
               s % apply some annotation, e.g. printing the solution
           ])
       )
   ;               
function ann: bab_min(var int: obj) = bab_min(obj, print);
function ann: bab_min(var int: obj, array[int] of ann: limits) =
       repeat(
           and([
               next(limits), % get next solution
               post(obj < sol(obj)), % post constraint
               print()
           ])
       )
   ;  

% Branch and Bound when maximising
function ann: bab_max(var int: obj, ann: s) =   
       repeat(
           and([
               next, % get next solution
               post(obj > sol(obj)), % post constraint
               s % apply some annotation, e.g. printing the solution
           ])
       )
   ;               
function ann: bab_max(var int: obj) = bab_max(obj, print());


% ------------------- Large Neighbourhood Search -------------------- %
% Large Neighbourhood Search variant that takes an objective variable
% and an array of search variables, a number of iterations and a 
% destruction rate d (that determines the percentage of variables in x 
% that will be destroyed, i.e. the size of the neighbourhood to search)
function ann: lns (var int: obj, array[int] of var int: x,
                   int: iterations, float: d) = 
    repeat (i in 1..iterations) (
      and([
        scope(
          and([ 
            post(neighbourhoodCts(x,d)),
            next, %bab_min(obj), %next, % or bab
            print()
          ]),
        ),
        print(),
        post(obj < sol(obj))
      ])
  );

function ann: lns (var int: obj, array[int] of var int: x,
                   int: iterations, float: d, float: secs) = 
    repeat (i in 1..iterations) (
      and([
        scope(
          or([
            and([ 
              post(neighbourhoodCts(x,d)),
              next(time_limit(secs)), %bab_min(obj), %next, % or bab
              print()
            ]),
            print()
          ])
        ),
        post(obj < sol(obj))
      ])
  );

% Predicate that randomly choses the neighbourhood to keep, and then 
% posts the respective assignment constraints (assigning those variables 
% to the value in the last solution)
predicate neighbourhoodCts(array[int] of var int: x, 
                            float: destructionRate) = 
   if(hasSol()) then 
     let {
       int: n = ceil((1.0 - destructionRate)*length(x));
       array[1..n] of int: idx = [ uniform(1,length(x)) | i in 1..n ];
     } in 
       forall (i in idx) (
         %trace("x["++show(i)++"] = "++show(sol(x[i]))++"\n") /\
         x[i] = sol(x[i])
       )     
   else true endif;


% ------------------- Two Stage Policy Based Search ------------------ %
function ann: policy_based_two_stage(array[int] of var int: stage1,
                                     array[int] of var int: stage2,
                                     var int: obj,
                                     int: nbScenarios) = 

   let { int: overall_obj = 0, % overall objective value
         array[1..nbScenarios] of int: scenario_obj = 
                                        [ 0 | i in 1..nbScenarios ]  } 
   in  
     and([
         repeat (
           and([
             next, % TODO: do we want the variable assignments to be fixed according to this solution in all subscopes?
               scope(
                 repeat (s in 1..nbScenarios) (
                   and([                     
                     post(secondStageConstraints(s)),
                     let {
                       var int: obj_s = getObjective(s);          
                     } in
                       and([bab_min(obj_s),
                           % scenario_obj[s] := val(obj) % does not work
                           comb_assign(scenario_obj[s], sol(obj_s))])
                   ]) 
                 )
              )
           ])
         ), 
       % overall_obj := sum (i in 1..nbScenarios) scenario_obj[i])
         comb_assign(overall_obj,
                   sum (i in 1..nbScenarios) (scenario_obj[i])),
         post(obj < overall_obj) % BAB constraint on overall obj var
     ])
;

% returns the objective variable for the given scenario 
function var int: getObjective (int: scenario);

% define this predicate in your model, defining the second stage 
% constraints for the given scenario
predicate secondStageConstraints(int: scenario); 

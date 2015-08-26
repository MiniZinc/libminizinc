%%%  MINISEARCH keywords and built-in meta-searches
include "globals.mzn";
% =================================================================== %
%                   MINISEARCH                                        %
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

function ann: '/\'(ann: a,ann: b) = and([a,b]);
function ann: '\/'(ann: a,ann: b) = or([a,b]);

% print the output as print()
function ann: print;
% print the string
function ann: print(string);
% get the next solution as next()
function ann: next; 
% do nothing: return SAT, skip()
function ann: skip;
% return UNSAT
function ann: fail;
% return UNKNOWN
function ann: prune;
% break()
function ann: break;

% assign search variable x to y
annotation comb_assign(int: x,int: y);

% assign search variable x to y
annotation comb_assign(float: x,float: y);

% assign search variable x to y
annotation comb_assign(bool: x,bool: y);

% assign solution variable s to result of c
annotation comb_assign(ann: s,ann: c);

% commit current solution to parent function scope
function ann: commit();

% break out of enclosing repeat loop
annotation break;

% do not remove the given array of variables during optimisation
% of FlatZinc, since it is used in MiniSearch
predicate keepAlive__(array [$U] of var $T);

% =================================================================== %
%                    the MiniSearch built meta-searches               %
% =================================================================== %

% Branch&Bound minimize combinator
include "bab_min.mzn";

% Branch and Bound when maximising
include "bab_max.mzn";

% --------- Lexicographic Optimisation ----------------------------%
function ann: minimize_lex(array[int] of var int: objs) =
  next() /\ commit() /\ print() /\
  repeat( scope( 
      let {array[1..length(objs)] of int:v=[ sol(objs[i])| i in 1..length(objs)]}
        in  post(lex_less(objs, v)) /\
            (next() /\ commit() /\ print())  \/  break
          )
   );


% --------- Dichotomic Search --------------------------------------%
function ann: minimize_dicho(var int: obj) =
let { int: low = lb(obj), int: high = ub(obj); } in
repeat(
   if (low = high) then
      break
   else
    scope(
     let { int: mid = low + floor((high-low)/2) } in
     post(low <= obj /\ obj <= mid) /\
     if next() then
       high := sol(obj) /\
%       print("SAT, new high: \(show(low))..*\(show(high))*\n") /\
       commit() /\ print()
     else
       low := mid+1
%       print("UNSAT, new low: *\(show(low))*..\(show(high))\n")
     endif
    )
endif
);

%% ------------------- Large Neighbourhood Search -------------------- %
%% Large Neighbourhood Search variant that takes an objective variable
%% and an array of search variables, a number of iterations and a 
%% destruction rate d (that determines the percentage of variables in x 
%% that will be destroyed, i.e. the size of the neighbourhood to search)

% --------- Randomized LNS ------------------------------------ %

function ann: lns_min (var int: obj, array[int] of var int: x,
                   int: iterations, float: d) = 
    repeat (i in 1..iterations) (
        scope(
            post(neighbourhoodCts(x,d)) /\
            next() /\ %bab_min(obj) /\
            commit() /\ 
            print("Intermediate solution with objective \(sol(obj))\n")
        ) /\
        post(obj < sol(obj))
   )
 ;

function ann: lns_min (var int: obj, array[int] of var int: x,
                   int: iterations, float: d, int: timeout_ms) = 
    repeat (i in 1..iterations) (
        scope(
            post(neighbourhoodCts(x,d)) /\
            time_limit(timeout_ms, 
                       bab_min(obj)) /\
            commit() /\
            print("Intermediate solution of LNS with objective \(sol(obj))\n")
         
        ) /\
        post(obj < sol(obj))
   )
;

%% Predicate that randomly choses the neighbourhood to keep, and then 
%% posts the respective assignment constraints (assigning those variables 
%% to the value in the last solution)
predicate neighbourhoodCts(array[int] of var int: x, 
                            float: destructionRate) = 
   if(hasSol()) then 
     let {
       int: n = ceil((1.0 - destructionRate)*length(x));
       array[1..n] of int: idx = [ uniform(1,length(x)) | i in 1..n ];
     } in
       forall (i in idx) (
          x[i] = sol(x[i])
       )     
   else true endif;


%---------- Adaptive LNS -----------------------------------%

function ann: adaptive_lns_min(var int: obj, array[int] of var int: vars,
                        int: iterations, int: initRate, int: exploreTime) =
  let { int: adaRate = initRate, int: step = 1; } in
  repeat (i in 1..iterations) (
    scope(
         (post(uniformNeighbourhood(vars,adaRate/100.0)) /\
          time_limit(exploreTime,
                bab_min(obj)) /\ 
          commit() /\ print() )
      \/ (adaRate := adaRate + step /\ fail)
    )
    /\ post(obj < sol(obj))
  );


predicate uniformNeighbourhood(array[int] of var int: x, float: destrRate) = 
  if hasSol() then
    forall(i in index_set(x)) (
      if uniform(0.0,1.0) > destrRate then 
        x[i] = sol(x[i]) 
      else true endif
    )
  else true endif;

% =================================================================== %
%                    search combinators MiniSearch                    %
% =================================================================== %
% ------------------- Branch & Bound -------------------------------- %
% Branch and Bound when maximizing
function ann: bab_max(var int: obj, ann: s) =   
   repeat(
       and([
           next(), % get next solution
           commit(),
           post(obj > sol(obj)), % post constraint
           s % apply some annotation, e.g. printing the solution
       ])
   )
   ;    

function ann: bab_max(var int: obj, array[int] of ann: limits) =
   repeat(
       and([
           next(limits), % get next solution
           commit(),
           post(obj > sol(obj)), % post constraint
           print()
       ])
   )
   ;  
           
%function ann: bab_max(var int: obj) = bab_max(obj, print());


function ann: bab_max(var int: obj) =   
   repeat(
       if next() then
         commit() /\ 
         print("Intermediate solution in BAB_MAX with objective \(sol(obj))\n") /\
         post(obj > sol(obj))
       else break endif
   )
;   

function ann: bab_max(var int: obj, int: timeout_ms) = 
   repeat(
       and([
           next(time_limit(timeout_ms)), % get next solution
           commit(),
           post(obj > sol(obj)), % post constraint
           print()
       ])
   )
   ;
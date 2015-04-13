% =================================================================== %
%                    search combinators MiniSearch                    %
% =================================================================== %
% ------------------- Branch & Bound -------------------------------- %
% Branch and Bound when minimizing
function ann: bab_min(var int: obj, ann: s) =   
   repeat(
       and([
           next(), % get next solution
           commit(),
           post(obj < sol(obj)), % post constraint
           s
       ])
   )
   ;               
function ann: bab_min(var int: obj, array[int] of ann: limits) =
   repeat(
       and([
           next(limits), % get next solution
           commit(),
           post(obj < sol(obj)), % post constraint
           print()
       ])
   )
   ;  

function ann: bab_min(var int: obj)  = bab_min(obj, print);

function ann: bab_min(var int: obj, int: timeout_ms) = 
   repeat(
       and([
           next(time_limit(timeout_ms)), % get next solution
           commit(),
           post(obj < sol(obj)), % post constraint
           print()
       ])
   )
   ;
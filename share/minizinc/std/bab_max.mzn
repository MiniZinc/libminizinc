% =================================================================== %
%                    search combinators MiniSearch                    %
% =================================================================== %
% ------------------- Branch & Bound -------------------------------- %
% Branch and Bound when maximising
function ann: bab_max(var int: obj) =   
   repeat(
       if next() then
         print("Intermediate solution in BAB-MAX with objective \(sol(obj))\n") /\
         commit() /\ 
         post(obj > sol(obj))
       else break endif
   );
function ann: bab_max(var int: obj, ann: s) =   
   repeat(
       and([
           next(), % get next solution
           commit(),
           post(obj > sol(obj)), % post constraint
           s % apply some annotation, e.g. printing the solution
       ])
   );    
function ann: bab_max(var int: obj, array[int] of ann: limits) =
   repeat(
       and([
           next(limits), % get next solution
           commit(),
           post(obj > sol(obj)), % post constraint
           print()
       ])
   );  

% =================================================================== %
%                    search combinators MiniSearch                    %
% =================================================================== %
% ------------------- Branch & Bound -------------------------------- %
% Branch and Bound when minimizing
function ann: bab_min(var int: obj) =   
   repeat(
       if next() then
         print("Intermediate solution in BAB-MIN with objective \(sol(obj))\n") /\        
         commit() /\ 
         post(obj < sol(obj))
       else break endif
   );   
   
function ann: bab_min(var int: obj, ann: s) =   
   repeat(
       if next() then
         print("Intermediate solution in BAB-MIN with objective \(sol(obj))\n") /\
         %s /\
         commit() /\ 
         post(obj < sol(obj))
       else break endif
   );   
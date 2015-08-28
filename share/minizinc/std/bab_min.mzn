% =================================================================== %
%                    search combinators MiniSearch                    %
% =================================================================== %
% ------------------- Branch & Bound -------------------------------- %
% Branch and Bound when minimizing
function ann: bab_min(var int: obj, bool: verbose) =   
   repeat(
       if next() then
         if verbose then 
            print("Intermediate solution in BAB-MIN with objective \(sol(obj))\n")
         else skip
         endif /\        
         commit() /\ 
         post(obj < sol(obj))
       else break endif
   );   
   
function ann: bab_min(var int: obj) =  bab_min(obj,false);

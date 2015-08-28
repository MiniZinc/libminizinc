% =================================================================== %
%                    search combinators MiniSearch                    %
% =================================================================== %
% ------------------- Branch & Bound -------------------------------- %
% Branch and Bound when minimizing
function ann: minimize_bab(var int: obj, bool: verbose) =   
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
   
function ann: minimize_bab(var int: obj) =  %minimize_bab(obj,false);
   repeat(
       if next() then
         commit() /\ 
         post(obj < sol(obj))
       else break endif
   );   
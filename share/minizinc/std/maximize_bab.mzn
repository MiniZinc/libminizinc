% =================================================================== %
%                    search combinators MiniSearch                    %
% =================================================================== %
% ------------------- Branch & Bound -------------------------------- %
% Branch and Bound when maximising
function ann: maximize_bab(var int: obj, bool: verbose) =   
   repeat(
       if next() then
         if verbose then 
            print("Intermediate solution in BAB-MAX with objective \(sol(obj))\n") 
         else skip
         endif /\
         commit() /\ 
         post(obj > sol(obj))
       else break endif
   );

function ann: maximize_bab(var int: obj) = 
   repeat(
       if next() then
         commit() /\ 
         post(obj > sol(obj))
       else break endif
   );
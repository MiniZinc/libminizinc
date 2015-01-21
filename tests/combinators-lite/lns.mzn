% Large Neighbourhood Search variant that takes an objective variable and 
% an array of search variables, a number of iterations and a destruction 
% rate d (that determines the percentage of variables in x that will be 
% destroyed, i.e. the size of the neighbourhood to search).
function ann: lns (int var: obj, array[int] of var int: x,
                   int: iterations, float: d) = 
  scope(
    repeat (i in 1..iterations) (
      scope(
        post(neighbourhoodCts(x,d)),
        next % or bab
      ),
      post(obj < sol(obj)
    )
  );

% Predicate that randomly choses the neighbourhood to keep, and then posts the
% respective assignment constraints (assigning those variables to the value in 
% the last solution).
predicate neighbourhoodCts(array[int] of var int: x, 
                            float: destructionRate) = 
   if(hasSol) then 
     let (
       int: n = ceil((1.0 - destructionRate)*length(x));
       array[1..n] of int: idx = [ uniform(1,length(x)) | i in 1..n ];
     ) in (
       forall (i in idx) (
         x[i] = sol(x[i])
       )
     )
   else true endif;
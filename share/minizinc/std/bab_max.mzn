% =================================================================== %
%                    search combinators MiniSearch                    %
% =================================================================== %
% ------------------- Branch & Bound -------------------------------- %
% Branch and Bound when maximizing
function ann: bab_max(var int: obj, ann: s) =   
   let {
      ann: cur = empty_model;
   } in (
       repeat(
           and([
               cur := next(), % get next solution
               commit(cur),
               post(obj > sol(cur,obj)), % post constraint
               s % apply some annotation, e.g. printing the solution
           ])
       )
   )
   ;    

function ann: bab_max(var int: obj, array[int] of ann: limits) =
   let {
      ann: cur = empty_model;
   } in (
       repeat(
           and([
               cur := next(limits), % get next solution
               commit(cur),
               post(obj > sol(cur,obj)), % post constraint
               print()
           ])
       )
   )
   ;  
           
function ann: bab_max(var int: obj) = bab_max(obj, print());

function ann: bab_max(var int: obj, int: timeout_ms) = 
   let {
      ann: cur = empty_model;
   } in (
       repeat(
           and([
               cur := next(time_limit(timeout_ms)), % get next solution
               commit(cur),
               post(obj > sol(cur,obj)), % post constraint
               print()
           ])
       )
   )
   ;
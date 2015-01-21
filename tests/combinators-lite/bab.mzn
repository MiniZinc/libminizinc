% Branch and Bound when minimizing
function ann: bab_min(var int: obj, ann: s) =   
   scope( 
       repeat(
           and([
               next, % get next solution
               post(obj < sol(obj), % post constraint
               s % apply some annotation, e.g. printing the solution
           ])
       )
   );               

function ann: bab_min(var int: obj) = bab_min(var int: obj, print);

% Branch and Bound when maximising
function ann: bab_max(var int: obj, ann: s) =   
   scope( 
       repeat(
           and([
               next, % get next solution
               post(obj > sol(obj), % post constraint
               s % apply some annotation, e.g. printing the solution
           ])
       )
   );               

function ann: bab_max(var int: obj) = bab_max(var int: obj, print);

% In the future we would like to give a predicate 'better' as argument
% (this is currently not possible due to language restrictions):
% function ann: bab(predicate: better , ann: s) = 
%   scope( 
%       repeat(
%           and([
%               next, 
%               post(better), 
%               s
%           ])
%       )
%   );
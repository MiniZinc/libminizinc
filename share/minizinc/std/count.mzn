%-----------------------------------------------------------------------------%
% Constrains 'c' to be the number of occurrences of 'y' in 'x'. 
%-----------------------------------------------------------------------------%

include "count_eq.mzn";

predicate count(array[int] of var int: x, var int: y, var int: c) =
    count_eq(x, y, c);

function var int: count(array[int] of var int: x, var int: y) =
  let { var 0..length(x): c; constraint count(x,y,c); } in c;

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%


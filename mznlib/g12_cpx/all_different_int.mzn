%
% File: minizinc/all_different_int.mzn
%
% Copyright:
%   2010-2011 NICTA
%
%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all different.
%-----------------------------------------------------------------------------%

predicate all_different_int(array[int] of var int: x);

%-----------------------------------------------------------------------------%

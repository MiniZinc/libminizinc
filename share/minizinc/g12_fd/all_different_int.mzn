%-----------------------------------------------------------------------------%
% Constrains the array of objects 'x' to be all different.
%-----------------------------------------------------------------------------%

predicate all_different_int(array[int] of var int: x) = 
    g12fd_int_all_different(x);

%-----------------------------------------------------------------------------%

% The implementation of the all_different constraint in the G12/FD solver.
% This should not be called directly, instead the definition above should
% be used.
predicate g12fd_int_all_different(array[int] of var int: x);

% G12/FD doesn't provide a reified all_different so we use the following
% definition if g12fd_int_all_different is called from a reified context.
%
predicate g12fd_int_all_different_reif(array[int] of var int: x, var bool: r) =
    r <-> forall(i,j in index_set(x) where i < j) ( x[i] != x[j] );

%-----------------------------------------------------------------------------%

% Nothing: a solver with set variables takes `std`'s own `set_search` and
% `warm_start`.
%
% The file exists so that `redefinitions.mzn`'s include resolves. A solver
% without set variables gets `fznso_nosets/`'s copy instead, which rewrites both
% onto the array of Booleans the set became.

% Shadows `std`'s file of the same name, and redefines no builtin.
%
% `std/nosets.mzn` turns set variables into arrays of Booleans by redefining the
% set builtins. Behind this interface that is not a model's decision to make:
% the solver says whether it has set variables, and a solver that does not gets
% `fznso_nosets/` ahead of `fznso_constraints/` on the include path, which
% supplies the same decompositions under the registry's names.
%
% Leaving `std`'s file to resolve would be worse than useless. Every set builtin
% is already rewritten in `redefinitions-fznso.mzn`, so a second library
% defining them is a duplicate definition rather than an override, and the model
% stops compiling — which is exactly what a model asking for `nosets.mzn` used
% to get.
%
% What it does keep is the machinery a model may use directly — `set2bools`,
% `setarray2bools`, `reverse_map_ab2si` — since asking for `nosets.mzn` is a
% reasonable way to reach those whether or not the solver has set variables.

include "fznso_set_to_bools.mzn";

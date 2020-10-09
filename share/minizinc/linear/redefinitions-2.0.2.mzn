% This file contains redefinitions of standard builtins for version 2.0.2
% that can be overridden by solvers.


predicate symmetry_breaking_constraint(var bool: b) =
    (b) %:: MIP_lazy %:: MIP_cut   %% MIP_cut wrong in CPLEX 12.6.3
    %% Symm breaking as lazy is 1% better in Gurobi 6.5.2 on the Challenges 2012-2015
    %% But caused a bug in 7.5.1 - switched off
    %% true    %% TO omit all symmetry_breaking_constraint's
    ;

%% Make sure no feasible solutions are cut off:
predicate redundant_constraint(var bool: b) =
  (b) %:: MIP_cut
  % true   %% To omit all redundant_constraint's
  ;

%% Linearized element: just call without shifting
predicate array_var_bool_element_nonshifted(var int: idx, array[int] of var bool: x, var bool: c) =
  array_var_bool_element(idx,x,c);

predicate array_var_int_element_nonshifted(var int: idx, array[int] of var int: x, var int: c) =
  array_var_int_element(idx,x,c);

predicate array_var_float_element_nonshifted(var int: idx, array[int] of var float: x, var float: c) =
  array_var_float_element(idx,x,c);

predicate array_var_set_element_nonshifted(var int: idx, array[int] of var set of int: x, var set of int: c) =
  array_var_set_element(idx,x,c);

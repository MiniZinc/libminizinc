% This file contains redefinitions of standard builtins for version 2.0.2
% that can be overridden by solvers.

/***
  @groupdef flatzinc.twootwo FlatZinc builtins added in MiniZinc 2.0.2.
  
  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.0.2. Solvers that support these natively need
  to include a file called redefinitions-2.0.2.mzn in their solver library that
  redefines these predicates as builtins.
  
*/

predicate symmetry_breaking_constraint(var bool: b) = b;

predicate redundant_constraint(var bool: b) = b;


/** @group flatzinc.twootwo Element constraint on array with MiniZinc index set, constrains \a x[\a idx] = \a c
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_bool_element_nonshifted(var int: idx, array[int] of var bool: x, var bool: c) =
  array_var_bool_element((idx-(min(index_set(x))-1))::domain,array1d(x),c);

/** @group flatzinc.twootwo Element constraint on array with MiniZinc index set, constrains \a x[\a idx] = \a c
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_int_element_nonshifted(var int: idx, array[int] of var int: x, var int: c) =
  array_var_int_element((idx-(min(index_set(x))-1))::domain,array1d(x),c);

/** @group flatzinc.twootwo Element constraint on array with MiniZinc index set, constrains \a x[\a idx] = \a c
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_float_element_nonshifted(var int: idx, array[int] of var float: x, var float: c) =
  array_var_float_element((idx-(min(index_set(x))-1))::domain,array1d(x),c);

/** @group flatzinc.twootwo Element constraint on array with MiniZinc index set, constrains \a x[\a idx] = \a c
  This can be overridden in a solver that can perform the index calculation more efficiently than using
  a MiniZinc decomposition.
*/
predicate array_var_set_element_nonshifted(var int: idx, array[int] of var set of int: x, var set of int: c) =
  array_var_set_element((idx-(min(index_set(x))-1))::domain,array1d(x),c);

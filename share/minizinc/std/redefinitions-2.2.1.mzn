% This file contains redefinitions of standard builtins for version 2.2.1
% that can be overridden by solvers.

/***
  @groupdef flatzinc.twotwoone FlatZinc builtins added in MiniZinc 2.2.1.

  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.2.1. Solvers that support these natively need
  to include a file called redefinitions-2.2.1.mzn in their solver library that
  redefines these predicates as builtins.

*/

/** @group flatzinc.twotwoone Constrains \a z = \(\a x ^ {\a y}\) */
predicate int_pow_fixed(var int: x, int: y, var int: z) =
  if y=0 then z=1 elseif y=1 then z=x else z=product([x | i in 1..y]) endif;


% This file contains redefinitions of standard builtins for version 2.2.1
% that can be overridden by solvers.

/** @group flatzinc.int Constrains \a z = \(\a x ^ {\a y}\) */
predicate int_pow_fixed(var int: x, int: y, var int: z) =
  if y=0 then z=1 elseif y=1 then z=x else z=product([x | i in 1..y]) endif;


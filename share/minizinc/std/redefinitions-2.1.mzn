/***
  @groupdef flatzinc.twoone FlatZinc builtins added in MiniZinc 2.1.0.
  
  These functions and predicates define built-in operations of the MiniZinc language
  that have been added in MiniZinc 2.1.0. Solvers that support these natively need
  to include a file called redefinitions-2.1.0.mzn in their solver library that
  redefines these predicates as builtins.
  
*/


/** @group flatzinc.twoone Constrains \a a ≤ \a x ≤ \a b */
predicate float_in(var float: x, float: a, float: b) =
  x >= a /\ x <= b;

/** @group flatzinc.twoone Constrains \a x to take one of the values in \a as */
predicate float_dom(var float: x, array[int] of float: as) =
  let {
    array[int] of var bool: b = [float_in(x,as[2*i-1],as[2*i]) | i in 1..length(as) div 2]
  } in exists(b);

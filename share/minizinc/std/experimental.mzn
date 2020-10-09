%-----------------------------------------------------------------------------%
% MiniZinc standard library.
% Experimental stuff.
%-----------------------------------------------------------------------------%
% This file contains declarations of all functions, predicates and annotations
% available in the base MiniZinc language.

/***
 @groupdef MAIN The MiniZinc library
*/


/***
 @groupdef annotations.multiobj-experimental Multiobjective annotations
*/

/** @group annotations.multiobj-experimental Sequentially (lexicographically) optimize objectives specified in array \a s.
  The \a goal_hierarchy annotation is to be attached to the solve item, for example:
  
  \code
  solve
    :: goal_hierarchy([int_min_goal(load[1]), int_min_goal(load[2]), int_min_goal(load[3])])
    satisfy;
  \endcode

  */
annotation goal_hierarchy(array[int] of ann);

/** @group annotations.multiobj-experimental Possible arguments of the \a goal_hierarchy annotation */
annotation min_goal(var int: x);
annotation min_goal(var float: x);
annotation int_min_goal(var int: x);
annotation float_min_goal(var float: x);
annotation max_goal(var int: x);
annotation max_goal(var float: x);
annotation int_max_goal(var int: x);
annotation float_max_goal(var float: x);
annotation sat_goal(var bool: b);


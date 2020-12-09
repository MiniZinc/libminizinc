/***
 @groupdef stdlib.annotations Annotations

 These annotations control evaluation and solving behaviour.
*/


/***
 @groupdef stdlib.annotations.general General annotations
*/

/** @group stdlib.annotations.general Declare function as total, i.e. it does not put
  any constraints on its arguments. */
annotation promise_total;

/** @group stdlib.annotations.general Declare that expression may have undefined result (to avoid warnings) */
annotation maybe_partial;

/** @group stdlib.annotations.general Declare that the annotated variable should be added to the output
of the model. This annotation only has an effect when the model does not have an output item. */
annotation add_to_output;

/** @group stdlib.annotations.general Declare that the annotated variable should be only used for output.
  This annotation can be used to define variables that are required for solution checkers, or
  that are necessary for the output item. The annotated variable must be par.
 */
annotation output_only;

/** @group stdlib.annotations.general Declare that the annotated variable is required for checking solutions. */
annotation mzn_check_var;

/** @group stdlib.annotations.general Declare that the annotated variable is required for checking solutions and has an enum type. */
annotation mzn_check_enum_var(array[int] of set of int);

/** @group stdlib.annotations.general Declare a name for the annotated expression. */
annotation mzn_expression_name(string);

/** @group stdlib.annotations.general Declare a name for the annotated constraint. */
annotation mzn_constraint_name(string);

/** @group stdlib.annotations.general State that a function is deprecated. */
annotation mzn_deprecated(string: version, string: explanation);

function $T: mzn_deprecate(string: name, string: version, string: msg, $T: x);
function var $T: mzn_deprecate(string: name, string: version, string: msg, var $T: x);
function var opt $T: mzn_deprecate(string: name, string: version, string: msg, var opt $T: x);
function array[$U] of $T: mzn_deprecate(string: name, string: version, string: msg, array[$U] of $T: x);
function array[$U] of var $T: mzn_deprecate(string: name, string: version, string: msg, array[$U] of var $T: x);
function array[$U] of var opt $T: mzn_deprecate(string: name, string: version, string: msg, array[$U] of var opt $T: x);

/** @group stdlib.annotations.general Declare the annotated variable as being functionally defined.
  This annotation is introduced into FlatZinc code by the compiler. */
annotation is_defined_var;
/** @group stdlib.annotations.general Declare a variable as being introduced by the compiler. */
annotation var_is_introduced;
/** @group stdlib.annotations.general Declare variable: \a c as being functionally defined
  by the annotated constraint. This annotation is introduced into FlatZinc code by the compiler. */
annotation defines_var(var $t: c);
/** @group stdlib.annotations.general Declare that the annotated array should be printed by
  the solver with the given index sets \a a. This annotation is introduced into FlatZinc code by
  the compiler. */
annotation output_array(array[$u] of set of int:a);
/** @group stdlib.annotations.general Declare that the annotated variable should be printed by
  the solver. This annotation is introduced into FlatZinc code by
  the compiler. */
annotation output_var;
/** @group stdlib.annotations.general Declare that the annotated expression is used to map
  an expression back from FlatZinc to MiniZinc.  */
annotation is_reverse_map;
/** @group stdlib.annotations.general Document the function or variable declaration item with
  the string \a s. */
annotation doc_comment(string: s);
/** @group stdlib.annotations.general Representation of the call-stack when the annotated item
  was introduced, as a string \a s. Can be used to uniquely identify variables and constraints across different
  compilations of a model that may have different names. This annotations is introduced into
  FlatZinc code by the compiler and is retained if --keep-paths argument is used. */
annotation mzn_path(string: s);
/** @group stdlib.annotations.general Used to attach a name \a s to an expression, this should also propagate to
  any sub-expressions or decomposition of the annotated expression. String annotations on expressions
  are re-written as expression_name annotations */
annotation expression_name(string: s);
/** @group stdlib.annotations.general Used to attach a name \a s to a constraint and its decomposition. String
  annotations on constraint keywords are re-written as constraint_name annotations */
annotation constraint_name(string: s);

/** @group stdlib.annotations.general Used internally by the compiler */
annotation mzn_rhs_from_assignment;

/** @group stdlib.annotations.general Marks a constraint as a recorded domain changing constraint (when mzn2fzn
  called with -g flag */
annotation domain_change_constraint;

/***
 @groupdef stdlib.annotations.prop Propagation strength annotations
*/

/** @group stdlib.annotations.prop Annotate a constraint to use domain propagation */
annotation domain;
/** @group stdlib.annotations.prop Annotate a constraint to use bounds propagation */
annotation bounds;



/***
 @groupdef stdlib.annotations.search Search annotations
*/

/** @group stdlib.annotations.search Sequentially perform the searches specified in array \a s */
annotation seq_search(array[int] of ann: s);

/** @group stdlib.annotations.search Specify search on variables \a x, with variable selection
  strategy \a select, value choice strategy \a choice, and exploration strategy
  \a explore. If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation int_search(
    array[$X] of var int: x,
    ann: select,
    ann: choice,
    ann: explore,
);

/** @group stdlib.annotations.search Specify search on variables \a x, with variable selection
  strategy \a select, and value choice strategy \a choice.
  If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation int_search(
    array[$X] of var int: x,
    ann: select,
    ann: choice
) = int_search(x,select,choice,complete);

/** @group stdlib.annotations.search Search annotation for optional integer variables */
annotation int_search(array[int] of var opt int: x, ann: a1, ann: a2, ann: a3) =
  int_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2,a3);

/** @group stdlib.annotations.search Search annotation for optional integer variables */
annotation int_search(array[int] of var opt int: x, ann: a1, ann: a2) =
  int_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2);

/** @group stdlib.annotations.search Specify search on variables \a x, with variable selection
  strategy \a select, value choice strategy \a choice, and exploration strategy
  \a explore.
  If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation bool_search(
    array[$X] of var bool: x,
    ann: select,
    ann: choice,
    ann: explore
);

/** @group stdlib.annotations.search Specify search on variables \a x, with variable selection
  strategy \a select, and value choice strategy \a choice.
  If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation bool_search(
    array[$X] of var bool: x,
    ann: select,
    ann: choice
) = bool_search(x,select,choice,complete);

/** @group stdlib.annotations.search Search annotation for optional Boolean variables */
annotation bool_search(array[int] of var opt bool: x, ann: a1, ann: a2, ann: a3) =
  bool_search([if occurs(x[i]) then deopt(x[i]) else false endif | i in index_set(x)],a1,a2,a3);

/** @group stdlib.annotations.search Search annotation for optional Boolean variables */
annotation bool_search(array[int] of var opt bool: x, ann: a1, ann: a2) =
  bool_search([if occurs(x[i]) then deopt(x[i]) else false endif | i in index_set(x)],a1,a2);

/** @group stdlib.annotations.search Specify search on variables \a x, 
  with precision \a prec, variable selection
  strategy \a select, value choice strategy \a choice, and exploration strategy
  \a explore.
  If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation float_search(
    array[$X] of var float: x,
    float: prec,
    ann: select,
    ann: choice,
    ann: explore
);

/** @group stdlib.annotations.search Specify search on variables \a x, 
  with precision \a prec, variable selection
  strategy \a select, and value choice strategy \a choice.
  If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation float_search(
    array[$X] of var float: x,
    float: prec,
    ann: select,
    ann: choice
) = float_search(x,prec,select,choice,complete);

/** @group stdlib.annotations.search Search annotation for optional float variables */
annotation float_search(array[int] of var opt float: x, ann: a1, ann: a2, ann: a3) =
  float_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2,a3);

/** @group stdlib.annotations.search Search annotation for optional float variables */
annotation float_search(array[int] of var opt float: x, ann: a1, ann: a2) =
  float_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2);

/** @group stdlib.annotations.search Specify search on variables \a x, with variable selection
  strategy \a select, value choice strategy \a choice, and exploration strategy
  \a explore.
  If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation set_search(
    array[$X] of var set of int: x,
    ann: select,
    ann: choice,
    ann: explore
);

/** @group stdlib.annotations.search Specify search on variables \a x, with variable selection
  strategy \a select, and value choice strategy \a choice.
  If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation set_search(
    array[$X] of var set of int: x,
    ann: select,
    ann: choice
) = set_search(x,select,choice,complete);

/***
 @groupdef stdlib.annotations.search.varsel Variable selection annotations
*/

/** @group stdlib.annotations.search.varsel Search variables in the given order */
annotation input_order;
/** @group stdlib.annotations.search.varsel Choose the variable with the smallest domain */
annotation first_fail;
/** @group stdlib.annotations.search.varsel Choose the variable with the largest domain */
annotation anti_first_fail;
/** @group stdlib.annotations.search.varsel Choose the variable with the smallest value in its domain */
annotation smallest;
/** @group stdlib.annotations.search.varsel Choose the variable with the largest value in its domain */
annotation largest;
/** @group stdlib.annotations.search.varsel Choose the variable with the largest number of attached constraints */
annotation occurrence;
/** @group stdlib.annotations.search.varsel Choose the variable with the smallest domain,
  breaking ties using the number of attached constraints */
annotation most_constrained;
/** @group stdlib.annotations.search.varsel Choose the variable with largest difference
  between the two smallest values in its domain  */
annotation max_regret;
/** @group stdlib.annotations.search.varsel Choose the variable with largest domain, divided
  by the number of attached constraints weighted by how often they have caused failure */
annotation dom_w_deg;
/** @group stdlib.annotations.search.varsel Choose the variable with the highest impact so
  far during the search  */
annotation impact;

/***
 @groupdef stdlib.annotations.search.choice Value choice annotations
*/

/** @group stdlib.annotations.search.choice Assign values in ascending order */
annotation indomain;
/** @group stdlib.annotations.search.choice Assign the smallest value in the domain */
annotation indomain_min;
/** @group stdlib.annotations.search.choice Assign the largest value in the domain */
annotation indomain_max;
/** @group stdlib.annotations.search.choice Assign the value in the domain closest to
  the mean of its current bounds */
annotation indomain_middle;
/** @group stdlib.annotations.search.choice Assign the middle value in the domain, or
  the smaller of the two middle values in case of an even number of elements in the domain */
annotation indomain_median;
/** @group stdlib.annotations.search.choice Assign a random value from the domain */
annotation indomain_random;
/** @group stdlib.annotations.search.choice Bisect the domain, excluding the upper half first */
annotation indomain_split;
/** @group stdlib.annotations.search.choice Bisect the domain, randomly selecting which half to exclude first */
annotation indomain_split_random;
/** @group stdlib.annotations.search.choice Bisect the domain, excluding the lower half first */
annotation indomain_reverse_split;
/** @group stdlib.annotations.search.choice 
  If the domain consists of several contiguous intervals, reduce the
  domain to the first interval. Otherwise bisect the domain.
 */
annotation indomain_interval;
/** @group stdlib.annotations.search.choice Exclude the smallest value from the domain */
annotation outdomain_min;
/** @group stdlib.annotations.search.choice Exclude the largest value from the domain */
annotation outdomain_max;
/** @group stdlib.annotations.search.choice Exclude the middle value from the domain */
annotation outdomain_median;
/** @group stdlib.annotations.search.choice Exclude a random value from the domain */
annotation outdomain_random;

/***
 @groupdef stdlib.annotations.search.explore Exploration strategy annotations
*/

/** @group stdlib.annotations.search.explore Perform a complete search */
annotation complete;

/***
 @groupdef stdlib.annotations.search.restart Restart annotations
*/

/** @group stdlib.annotations.search.restart Restart with Luby sequence scaled by \a scale */
annotation restart_luby(int: scale);
/** @group stdlib.annotations.search.restart Restart with geometric sequence with parameters \a base and \a scale */
annotation restart_geometric(float: base, int: scale);
/** @group stdlib.annotations.search.restart Restart with linear sequence scaled by \a scale */
annotation restart_linear(int: scale);
/** @group stdlib.annotations.search.restart Restart after constant number of nodes \a scale */
annotation restart_constant(int: scale);
/** @group stdlib.annotations.search.restart Do not restart */
annotation restart_none;

/***
 @groupdef stdlib.annotations.warmstart Warm start annotations

 To be put on the solve item, similar to search annotations.
 A variable can be mentioned several times and in different
 annotations but only one of the values is taken
*/

/** @group stdlib.annotations.warmstart Specify an array \a w of warm_start annotations
 or other warm_start_array annotations. Can be useful to keep the annotation
 order in FlatZinc for manual updating.

 Note: if you have search annotations as well, put warm_starts into seq_search
 in order to have precedence between both, which may matter.
*/
annotation warm_start_array( array[int] of ann: w );

/** @group stdlib.annotations.warmstart Specify warm start values \a v for an array of booleans \a x */
annotation warm_start( array[int] of var bool: x, array[int] of bool: v );
/** @group stdlib.annotations.warmstart Specify warm start values \a v for an array of integers \a x */
annotation warm_start( array[int] of var int: x, array[int] of int: v );
/** @group stdlib.annotations.warmstart Specify warm start values \a v for an array of floats \a x */
annotation warm_start( array[int] of var float: x, array[int] of float: v );
/** @group stdlib.annotations.warmstart Specify warm start values \a v for an array of sets \a x */
annotation warm_start( array[int] of var set of int: x, array[int] of set of int: v );



/***
  @groupdef stdlib.special Special constraints

  These predicates allow users to mark constraints as e.g. symmetry breaking
  or redundant, so that solvers can choose to implement them differently.

  We cannot easily use annotations for this purpose, since annotations are
  propagated to all constraints in a decomposition, which may be incorrect
  for redundant or symmetry breaking constraints in the presence of
  common subexpression elimination (CSE).
*/

/** @group stdlib.special Mark \a b as a symmetry breaking constraint */
predicate symmetry_breaking_constraint(var bool: b);

/** @group stdlib.special Mark \a b as a redundant constraint */
predicate redundant_constraint(var bool: b);

/** @group stdlib.special Mark \a b as an implied constraint (synonym for redundant_constraint) */
predicate implied_constraint(var bool: b) = redundant_constraint(b);


%-----------------------------------------------------------------------------%
% MiniZinc standard library.
%-----------------------------------------------------------------------------%
% This file contains declarations of all functions, predicates and annotations
% available in the base MiniZinc language.

/***
 @groupdef MAIN The MiniZinc library
*/

/***
 @groupdef annotations Annotations
 
 These annotations control evaluation and solving behaviour.
*/


/***
 @groupdef annotations.general General annotations
*/

/** @group annotations.general Declare function as total, i.e. it does not put
  any constraints on its arguments. */
annotation promise_total;

/** @group annotations.general Declare that expression may have undefined result (to avoid warnings) */
annotation maybe_partial;

/** @group annotations.general Declare that the annotated variable should be added to the output
of the model. This annotation only has an effect when the model does not have an output item. */
annotation add_to_output;

/** @group annotations.general Declare that the annotated variable should be only used for output.
  This annotation can be used to define variables that are required for solution checkers, or
  that are necessary for the output item. The annotated variable must be par.
 */
annotation output_only;

/** @group annotations.general Declare that the annotated variable is required for checking solutions. */
annotation mzn_check_var;

/** @group annotations.general Declare that the annotated variable is required for checking solutions and has an enum type. */
annotation mzn_check_enum_var(set of int);

/** @group annotations.general Declare a name for the annotated expression. */
annotation mzn_expression_name(string);

/** @group annotations.general Declare a name for the annotated constraint. */
annotation mzn_constraint_name(string);


/** @group annotations.general Declare the annotated variable as being functionally defined.
  This annotation is introduced into FlatZinc code by the compiler. */
annotation is_defined_var;
/** @group annotations.general Declare a variable as being introduced by the compiler. */
annotation var_is_introduced;
/** @group annotations.general Declare variable: \a c as being functionally defined
  by the annotated constraint. This annotation is introduced into FlatZinc code by the compiler. */
annotation defines_var(var $t: c);
/** @group annotations.general Declare that the annotated array should be printed by
  the solver with the given index sets \a a. This annotation is introduced into FlatZinc code by
  the compiler. */
annotation output_array(array[$u] of set of int:a);
/** @group annotations.general Declare that the annotated variable should be printed by
  the solver. This annotation is introduced into FlatZinc code by
  the compiler. */
annotation output_var;
/** @group annotations.general Declare that the annotated expression is used to map
  an expression back from FlatZinc to MiniZinc.  */
annotation is_reverse_map;
/** @group annotations.general Document the function or variable declaration item with
  the string \a s. */
annotation doc_comment(string: s);
/** @group annotations.general Representation of the call-stack when the annotated item
  was introduced, as a string \a s. Can be used to uniquely identify variables and constraints across different
  compilations of a model that may have different names. This annotations is introduced into
  FlatZinc code by the compiler and is retained if --keep-paths argument is used. */
annotation mzn_path(string: s);
/** @group annotations.general With debug build of mzn2fzn, call MiniZinc::mzn_break_here when
  flattening this expression to make debugging easier. This annotation is ignored by the
  release build.*/
annotation mzn_break_here;
/** @group annotations.general Used to attach a name \a s to an expression, this should also propagate to
  any sub-expressions or decomposition of the annotated expression. String annotations on expressions
  are re-written as expression_name annotations */
annotation expression_name(string: s);
/** @group annotations.general Used to attach a name \a s to a constraint and its decomposition. String
  annotations on constraint keywords are re-written as constraint_name annotations */
annotation constraint_name(string: s);

/** @group annotations.general Used internally by the compiler */
annotation mzn_rhs_from_assignment;

/** @group annotations.general Marks a constraint as a recorded domain changing constraint (when mzn2fzn
  called with -g flag */
annotation domain_change_constraint;

/***
 @groupdef annotations.prop Propagation strength annotations
*/

/** @group annotations.prop Annotate a constraint to use domain propagation */
annotation domain;
/** @group annotations.prop Annotate a constraint to use bounds propagation */
annotation bounds;


/***
 @groupdef annotations.search Search annotations
*/

/** @group annotations.search Sequentially perform the searches specified in array \a s */
annotation seq_search(array[int] of ann: s);

/** @group annotations.search Specify search on variables \a x, with variable selection
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

/** @group annotations.search Specify search on variables \a x, with variable selection
  strategy \a select, and value choice strategy \a choice.
  If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation int_search(
    array[$X] of var int: x,
    ann: select,
    ann: choice
) = int_search(x,select,choice,complete);

/** @group annotations.search Specify search on variables \a x, with variable selection
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

/** @group annotations.search Specify search on variables \a x, with variable selection
  strategy \a select, and value choice strategy \a choice.
  If \a x is a multi-dimensional array, it is coerced to one-dimensional
  in row-major order (as with the array1d function).
*/
annotation bool_search(
    array[$X] of var bool: x,
    ann: select,
    ann: choice
) = bool_search(x,select,choice,complete);

/** @group annotations.search Specify search on variables \a x, 
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

/** @group annotations.search Specify search on variables \a x, 
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

/** @group annotations.search Specify search on variables \a x, with variable selection
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

/** @group annotations.search Specify search on variables \a x, with variable selection
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
 @groupdef annotations.search.varsel Variable selection annotations
*/

/** @group annotations.search.varsel Search variables in the given order */
annotation input_order;
/** @group annotations.search.varsel Choose the variable with the smallest domain */
annotation first_fail;
/** @group annotations.search.varsel Choose the variable with the largest domain */
annotation anti_first_fail;
/** @group annotations.search.varsel Choose the variable with the smallest value in its domain */
annotation smallest;
/** @group annotations.search.varsel Choose the variable with the largest value in its domain */
annotation largest;
/** @group annotations.search.varsel Choose the variable with the largest number of attached constraints */
annotation occurrence;
/** @group annotations.search.varsel Choose the variable with the smallest domain,
  breaking ties using the number of attached constraints */
annotation most_constrained;
/** @group annotations.search.varsel Choose the variable with largest difference
  between the two smallest values in its domain  */
annotation max_regret;
/** @group annotations.search.varsel Choose the variable with largest domain, divided
  by the number of attached constraints weighted by how often they have caused failure */
annotation dom_w_deg;
/** @group annotations.search.varsel Choose the variable with the highest impact so
  far during the search  */
annotation impact;

/***
 @groupdef annotations.search.choice Value choice annotations
*/

/** @group annotations.search.choice Assign values in ascending order */
annotation indomain;
/** @group annotations.search.choice Assign the smallest value in the domain */
annotation indomain_min;
/** @group annotations.search.choice Assign the largest value in the domain */
annotation indomain_max;
/** @group annotations.search.choice Assign the value in the domain closest to
  the mean of its current bounds */
annotation indomain_middle;
/** @group annotations.search.choice Assign the middle value in the domain */
annotation indomain_median;
/** @group annotations.search.choice Assign a random value from the domain */
annotation indomain_random;
/** @group annotations.search.choice Bisect the domain, excluding the upper half first */
annotation indomain_split;
/** @group annotations.search.choice Bisect the domain, randomly selecting which half to exclude first */
annotation indomain_split_random;
/** @group annotations.search.choice Bisect the domain, excluding the lower half first */
annotation indomain_reverse_split;
/** @group annotations.search.choice 
  If the domain consists of several contiguous intervals, reduce the
  domain to the first interval. Otherwise bisect the domain.
 */
annotation indomain_interval;
/** @group annotations.search.choice Exclude the smallest value from the domain */
annotation outdomain_min;
/** @group annotations.search.choice Exclude the largest value from the domain */
annotation outdomain_max;
/** @group annotations.search.choice Exclude the middle value from the domain */
annotation outdomain_median;
/** @group annotations.search.choice Exclude a random value from the domain */
annotation outdomain_random;

/***
 @groupdef annotations.search.explore Exploration strategy annotations
*/

/** @group annotations.search.explore Perform a complete search */
annotation complete;

/***
 @groupdef annotations.search.restart Restart annotations
*/

/** @group annotations.search.restart Restart with Luby sequence scaled by \a scale */
annotation restart_luby(int: scale);
/** @group annotations.search.restart Restart with geometric sequence with parameters \a base and \a scale */
annotation restart_geometric(float: base, int: scale);
/** @group annotations.search.restart Restart with linear sequence scaled by \a scale */
annotation restart_linear(int: scale);
/** @group annotations.search.restart Restart after constant number of nodes \a scale */
annotation restart_constant(int: scale);
/** @group annotations.search.restart Do not restart */
annotation restart_none;

/***
 @groupdef annotations.warmstart Warm start annotations

 To be put on the solve item, similar to search annotations.
 A variable can be mentioned several times and in different
 annotations but only one of the values is taken
*/

/** @group annotations.warmstart Specify an array \a w of warm_start annotations
 or other warm_start_array annotations. Can be useful to keep the annotation
 order in FlatZinc for manual updating.
 
 Note: if you have search annotations as well, put warm_starts into seq_search
 in order to have precedence between both, which may matter.
*/
annotation warm_start_array( array[int] of ann: w );

/** @group annotations.warmstart Specify warm start values \a v for an array of booleans \a x */
annotation warm_start( array[int] of var bool: x, array[int] of bool: v );
/** @group annotations.warmstart Specify warm start values \a v for an array of integers \a x */
annotation warm_start( array[int] of var int: x, array[int] of int: v );
/** @group annotations.warmstart Specify warm start values \a v for an array of floats \a x */
annotation warm_start( array[int] of var float: x, array[int] of float: v );
/** @group annotations.warmstart Specify warm start values \a v for an array of sets \a x */
annotation warm_start( array[int] of var set of int: x, array[int] of set of int: v );


/***
 @groupdef annotations.warmstart.optvals Warm start annotations with optional values

 The value arrays can contain <> elements (absent values).
 The following decompositions eliminate those elements
 because FlatZinc 1.6 does not support optionals.
*/

/** @group annotations.warmstart.optvals Specify warm start values \a v for an array of booleans \a x */
annotation warm_start( array[int] of var bool: x, array[int] of opt bool: v ) =
    if 0==length(x) \/ 0==length(v) then
        warm_start( x, [] )
    else
        warm_start( [ x[i] | i in index_set(x) where
            i-min(index_set(x))+min(index_set(v))>length(v) \/
            occurs( v[ i-min(index_set(x))+min(index_set(v)) ] ) ],
            [ deopt(v[i]) | i in index_set(v) where occurs(v[i]) ] )
    endif;
/** @group annotations.warmstart.optvals Specify warm start values \a v for an array of integers \a x */
annotation warm_start( array[int] of var int: x, array[int] of opt int: v ) =
    if 0==length(x) \/ 0==length(v) then
        warm_start( x, [] )
    else
        warm_start( [ x[i] | i in index_set(x) where
            i-min(index_set(x))+min(index_set(v))>length(v) \/
            occurs( v[ i-min(index_set(x))+min(index_set(v)) ] ) ],
            [ deopt(v[i]) | i in index_set(v) where occurs(v[i]) ] )
    endif;
/** @group annotations.warmstart.optvals Specify warm start values \a v for an array of floats \a x */
annotation warm_start( array[int] of var float: x, array[int] of opt float: v ) =
    if 0==length(x) \/ 0==length(v) then
        warm_start( x, [] )
    else
        warm_start( [ x[i] | i in index_set(x) where
            i-min(index_set(x))+min(index_set(v))>length(v) \/
            occurs( v[ i-min(index_set(x))+min(index_set(v)) ] ) ],
            [ deopt(v[i]) | i in index_set(v) where occurs(v[i]) ] )
    endif;
/** @group annotations.warmstart.optvals Specify warm start values \a v for an array of sets \a x */
annotation warm_start( array[int] of var set of int: x, array[int] of opt set of int: v ) =
    if 0==length(x) \/ 0==length(v) then
        warm_start( x, [] )
    else
        warm_start( [ x[i] | i in index_set(x) where
            i-min(index_set(x))+min(index_set(v))>length(v) \/
            occurs( v[ i-min(index_set(x))+min(index_set(v)) ] ) ],
            [ deopt(v[i]) | i in index_set(v) where occurs(v[i]) ] )
    endif;



/***
  @groupdef optiontypes Option type support
  
  These functions and predicates implement the standard library for working
  with option types. Note that option type support is still incomplete.
*/

/** @group optiontypes Return value of \a x if \a x is not absent. Aborts 
  when evaluated on absent value. */
function $T: deopt(opt $T: x);

/** @group optiontypes Return value \a x unchanged (since \a x is guaranteed
  to be non-optional). */
function var $T: deopt(var $T: x) = x;

/** @group optiontypes Test if \a x is not absent (always returns true) */
test occurs(var $T: x) = true;
/** @group optiontypes Test if \a x is not absent */
test occurs(opt $T: x);
/** @group optiontypes Test if \a x is absent (always returns false) */
test absent(var $T: x) = false;
/** @group optiontypes Test if \a x is absent */
test absent(opt $T: x) = not occurs(x);

/***
  @groupdef optiontypes.bool Option type support for Booleans
*/

/** @group optiontypes.bool True iff \a x is not absent */
function var bool : occurs(var opt bool: x) ::promise_total = occurs_bool(x);

function var bool : occurs_bool(var opt bool: x) ::promise_total =
  let {
    var bool : b = occurs_internal_bool(x);
    var bool : dx = deopt_internal_bool(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in b;
  
/** @group optiontypes.bool Return value of \a x (assumes that \a x is not absent) */
function var bool : deopt(var opt bool : x) ::promise_total = deopt_bool(x);

function var bool : deopt_bool(var opt bool : x) ::promise_total =
  let {
    var bool : b = occurs_internal_bool(x);
    var bool : dx = deopt_internal_bool(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in dx;

/** @group optiontypes.bool True iff \a x is absent */
predicate absent(var opt bool: x) = not occurs(x);

function var bool: occurs_internal_bool(var opt bool: x) ::promise_total =
  let { var bool : b; } in b;
function var bool : deopt_internal_bool(var opt bool : x) ::promise_total =
  let { var bool: y } in y;

function var opt bool: reverse_map(var bool: occ, var bool: d);
function opt bool: reverse_map(bool: occ, bool: d) ::promise_total =
  if occ then d else <> endif;

predicate bool_opt_eq(var opt bool: x, var opt bool: y) =
  deopt(x)=deopt(y) /\ occurs(x)=occurs(y);

/** @group optiontypes.bool True iff both \a b0 and \a b1 are absent or
  both are present and have the same value. */
predicate bool_eq(var opt bool: b0, var opt bool: b1) =
     (absent(b0) /\ absent(b1))
  \/ (occurs(b0) /\ occurs(b1) /\ deopt(b0)=deopt(b1));

/** @group optiontypes.bool True iff \a b0 occurs and is equal to \a b1 */
predicate bool_eq(var opt bool: b0, var bool: b1) =
  occurs(b0) /\ deopt(b0)=b1;

  /** @group optiontypes.bool True iff \a b1 occurs and is equal to \a b0 */
predicate bool_eq(var bool: b0, var opt bool: b1) =
  occurs(b1) /\ deopt(b1)=b0;

/** @group optiontypes.bool True iff for any \p i, \a x[i] is absent or true */
predicate forall (array[int] of var opt bool: x) =
  forall ([absent(x[i]) \/ deopt(x[i]) | i in index_set(x)]);

/** @group optiontypes.bool True iff for at least one \p i, \a x[i] occurs and is true */
predicate exists (array[int] of var opt bool: x) =
  exists ([occurs(x[i]) /\ deopt(x[i]) | i in index_set(x)]);

/** @group optiontypes.bool True iff \a x is absent or false */
function var bool: 'not'(var opt bool: x) = absent(x) \/ not deopt(x);

/** @group optiontypes.bool Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt bool: element(var opt int: idx, array[int] of var bool: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group optiontypes.bool Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt bool: element(var opt int: idx1, var opt int: idx2, array[int,int] of var bool: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group optiontypes.bool Return \a x[\a idx] */
function var opt bool: element(var int: idx, array[int] of var opt bool: x) =
  let {
    var opt bool: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

/** @group optiontypes.bool Return \a x[\a idx1, \a idx2] */
function var opt bool: element(var int: idx1, var int: idx2, array[int,int] of var opt bool: x) =
  let {
    var opt bool: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

/** @group optiontypes.bool Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt bool: element(var opt int: idx, array[int] of var opt bool: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group optiontypes.bool Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt bool: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt bool: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group optiontypes.bool Search annotation for optional Boolean variables */
annotation bool_search(array[int] of var opt bool: x, ann: a1, ann: a2, ann: a3) =
  bool_search([if occurs(x[i]) then deopt(x[i]) else false endif | i in index_set(x)],a1,a2,a3);

/** @group optiontypes.bool Search annotation for optional Boolean variables */
annotation bool_search(array[int] of var opt bool: x, ann: a1, ann: a2) =
  bool_search([if occurs(x[i]) then deopt(x[i]) else false endif | i in index_set(x)],a1,a2);


/***
  @groupdef optiontypes.int Option type support for integers
*/

/** @group optiontypes.int True iff \a x is not absent */
function var bool : occurs(var opt int : x) ::promise_total = occurs_int(x);

function var bool : occurs_int(var opt int : x) ::promise_total =
  let {
    var bool : b = occurs_internal_int(x);
    var int  : dx = deopt_internal_int(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in b;

/** @group optiontypes.int Return value of \a x (assumes that \a x is not absent) */
function var $$E : deopt(var opt $$E : x) ::promise_total = deopt_int(x);

function var int : deopt_int(var opt int : x) ::promise_total =
  let {
    var bool : b = occurs_internal_int(x);
    var int  : dx = deopt_internal_int(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in dx;

/** @group optiontypes.int True iff \a x is absent */
function var bool: absent(var opt int: x) ::promise_total = not occurs(x);

function var bool: occurs_internal_int(var opt int: x) ::promise_total =
  let { var bool : b; } in b;
function var int : deopt_internal_int(var opt int : x) ::promise_total =
  let { var lb(x)..ub(x): y } in y;

function var opt int: reverse_map(var bool: occ, var int: d);
function opt int: reverse_map(bool: occ, int: d) ::promise_total =
  if occ then d else <> endif;

predicate var_dom(var opt int:x, set of int: s) =
  let {
    var int: dx = deopt(x);
    set of int: new_dom = dom(dx) intersect s;
  } in if new_dom = {} then absent(x) else dx in new_dom endif;

predicate var_dom(array[$T] of var opt int: x, set of int: d) =
    let { array[int] of var opt int: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));

predicate int_opt_eq(var opt int: x, var opt int: y) =
  deopt(x) = deopt(y) /\ occurs(x) = occurs(y);

/** @group optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function var bool: '>'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function var bool: '>='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function var bool: '<'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function var bool: '<='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function bool: '>'(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function bool: '>='(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function bool: '<'(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group optiontypes.int Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function bool: '<='(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group optiontypes.int Return minimum of elements in \a x that are not absent, or
absent if all elements in \a x are absent. */
function var opt int: min(array[int] of var opt int: x) ::promise_total =
    let {
        var opt lb_array(x)..ub_array(x): m;
        var lb_array(x)..ub_array(x): xmax;
        constraint if ub(xmax) != infinity then xmax = ub(xmax) else forall (i in index_set(x)) (xmax >= deopt(x[i])) endif;
        constraint occurs(m) <-> exists (i in index_set(x)) (occurs(x[i]));
        constraint occurs(m) ->
            deopt(m) = min([if occurs(xi) then deopt(xi) else xmax endif | xi in x]);
    } in m;

/** @group optiontypes.int Return maximum of elements in \a x that are not absent, or
absent if all elements in \a x are absent. */
function var opt int: max(array[int] of var opt int: x) ::promise_total =
    let {
        var opt lb_array(x)..ub_array(x): m;
        var lb_array(x)..ub_array(x): xmin;
        constraint if lb(xmin) != -infinity then xmin = lb(xmin) else forall (i in index_set(x)) (xmin <= deopt(x[i])) endif;
        constraint occurs(m) <-> exists (i in index_set(x)) (occurs(x[i]));
        constraint occurs(m) ->
            deopt(m) = max([if occurs(xi) then deopt(xi) else xmin endif | xi in x]);
    } in m;

/** @group optiontypes.int Weak addition. Return sum of \a x and \a y if both
are present, otherwise return absent. */
function var opt int: '~+'(var opt int: x, var opt int: y) ::promise_total =
    let { 
    int: l = if lb(x)=-infinity \/ lb(y)=-infinity then -infinity else lb(x)+lb(y) endif;
    int: u = if ub(x)=infinity \/ ub(y)=infinity then infinity else ub(x)+ub(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)+deopt(y);
    } in result;

/** @group optiontypes.int Weak subtraction. Return difference of \a x and \a y if both
are present, otherwise return absent. */
function var opt int: '~-'(var opt int: x, var opt int: y) ::promise_total =
    let { 
    int: l = if lb(x)=-infinity \/ ub(y)=infinity then -infinity else lb(x)-ub(y) endif;
    int: u = if ub(x)=infinity \/ lb(y)=-infinity then infinity else ub(x)-lb(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)-deopt(y);
    } in result;

/** @group optiontypes.int Weak multiplication. Return product of \a x and \a y if both
are present, otherwise return absent. */
function var opt int: '~*'(var opt int: x, var opt int: y) ::promise_total =
    if absent(x) \/ absent(y) then <>
    else deopt(x)*deopt(y) endif;

/** @group optiontypes.int Weak equality. True if either \a x or \a y are absent, or
present and equal.*/
function var bool: '~='(var opt int: x, var opt int: y) ::promise_total =
    absent(x) \/ absent(y) \/ deopt(x)=deopt(y);

/** @group optiontypes.int Return optional 0/1 integer that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt int: bool2int(var opt bool: x) ::promise_total =
  let {
    var opt 0..1: xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=bool2int(deopt(x));
  } in xi;

/** @group optiontypes.int True iff both \a x and \a y are absent or
  both are present and have the same value. */
predicate int_eq(var opt int: x, var opt int: y) =
     (absent(x) /\ absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)=deopt(y))::maybe_partial);

/** @group optiontypes.int True iff only one of \a x and \a y is absent or
  both are present and have different values. */
predicate int_ne(var opt int : x, var opt int : y) =
     (absent(x) != absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)!=deopt(y))::maybe_partial);

/** @group optiontypes.int Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function var int: '+'(var opt int: x, var opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group optiontypes.int Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function int: '+'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group optiontypes.int Optional subtraction. Return difference of \a x and \a y, with absent replaced by 0. */
function var int: '-'(var opt int: x, var opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif - if occurs(y) then deopt(y) else 0 endif;

/** @group optiontypes.int Optional subtraction. Return difference of \a x and \a y, with absent replaced by 0. */
function int: '-'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif - if occurs(y) then deopt(y) else 0 endif;

/** @group optiontypes.int Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function var int: '*'(var opt int: x, var opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.int Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function int: '*'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.int Optional division. Return \a x div \a y, with absent replaced by 1. */
function var int: 'div'(var opt int: x, var opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif div if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.int Optional division. Return \a x div \a y, with absent replaced by 1. */
function int: 'div'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif div if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.int Optional modulo. Return \a x mod \a y, with absent replaced by 1. */
function var int: 'mod'(var opt int: x, var opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif mod if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.int Optional modulo. Return \a x mod \a y, with absent replaced by 1. */
function int: 'mod'(opt int: x, opt int: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif mod if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.int Return sum of non-absent elements of \a x. */
function var int: sum(array[int] of var opt int: x) =
  sum (i in index_set(x)) (let { var int: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 0 endif);

/** @group optiontypes.int Return sum of non-absent elements of \a x. */
function int: sum(array[int] of opt int: x) =
  sum (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 0 endif);

/** @group optiontypes.int Return product of non-absent elements of \a x. */
function var int: product(array[int] of var opt int: x) =
  product (i in index_set(x)) (let { var int: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 1 endif);

/** @group optiontypes.int Return product of non-absent elements of \a x. */
function int: product(array[int] of opt int: x) =
  product (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 1 endif);

/** @group optiontypes.int Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt int: element(var opt int: idx, array[int] of var int: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group optiontypes.int Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt int: element(var opt int: idx1, var opt int: idx2, array[int,int] of var int: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group optiontypes.int Return \a x[\a idx] */
function var opt int: element(var int: idx, array[int] of var opt int: x) =
  let {
    var opt int: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

/** @group optiontypes.int Return \a x[\a idx1, \a idx2] */
function var opt int: element(var int: idx1, var int: idx2, array[int,int] of var opt int: x) =
  let {
    var opt int: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

/** @group optiontypes.int Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt int: element(var opt int: idx, array[int] of var opt int: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group optiontypes.int Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt int: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt int: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group optiontypes.int Search annotation for optional integer variables */
annotation int_search(array[int] of var opt int: x, ann: a1, ann: a2, ann: a3) =
  int_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2,a3);

/** @group optiontypes.int Search annotation for optional integer variables */
annotation int_search(array[int] of var opt int: x, ann: a1, ann: a2) =
  int_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2);

/* Internal function used to optimize over option type objective */

function var int: objective_deopt_(var opt int: x, bool: direction) =
  let {
    int: worst = if direction then lb(x)-1 else ub(x)+1 endif;
  } in if occurs(x) then deopt(x) else worst endif;

/***
  @groupdef optiontypes.float Option type support for floats
*/

/** @group optiontypes.float True iff \a x is not absent */
function var bool : occurs(var opt float : x) ::promise_total = occurs_float(x);

function var bool : occurs_float(var opt float : x) ::promise_total =
  let {
    var bool : b = occurs_internal_float(x);
    var float : dx = deopt_internal_float(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in b;

/** @group optiontypes.float Return value of \a x (assumes that \a x is not absent) */
function var float : deopt(var opt float : x) ::promise_total = deopt_float(x);

function var float : deopt_float(var opt float : x) ::promise_total =
  let {
    var bool : b = occurs_internal_float(x);
    var float : dx = deopt_internal_float(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in dx;

/** @group optiontypes.float True iff \a x is absent */
function var bool: absent(var opt float: x) ::promise_total = not occurs(x);

function var bool: occurs_internal_float(var opt float: x) ::promise_total =
  let { var bool : b; } in b;
function var float : deopt_internal_float(var opt float : x) ::promise_total =
  let { var lb(x)..ub(x): y } in y;

function var opt float: reverse_map(var bool: occ, var float: d); % :NOTE: Must define in cpp?
function opt float: reverse_map(bool: occ, float: d) ::promise_total =
  if occ then d else <> endif;

% :NOTE: does it apply to float?
/*
predicate var_dom(var opt float:x, set of float: s) =
  let {
    var float: dx = deopt(x);
    set of float: new_dom = dom(dx) intersect s;
  } in if new_dom = {} then absent(x) else dx in new_dom endif;

predicate var_dom(array[$T] of var opt float: x, set of float: d) =
    let { array[int] of var opt float: xx = array1d(x) }
    in forall (i in index_set(xx)) (var_dom(xx[i],d));
*/

predicate float_opt_eq(var opt float: x, var opt float: y) =
  deopt(x) = deopt(y) /\ occurs(x) = occurs(y);

/** @group optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function var bool: '>'(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function var bool: '>='(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function var bool: '<'(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function var bool: '<='(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function bool: '>'(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function bool: '>='(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function bool: '<'(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group optiontypes.float Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function bool: '<='(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group optiontypes.float Return minimum of elements in \a x that are not absent, or
absent if all elements in \a x are absent. */
% :NOTE: since -oo is always a lb for float variables, should we instead return -oo if all absent?
function var opt float: min(array[int] of var opt float: x) ::promise_total =
    let {
        var opt lb_array(x)..ub_array(x): m;
        var lb_array(x)..ub_array(x): xmax;
        constraint if ub(xmax) != infinity then xmax = ub(xmax) else forall (i in index_set(x)) (xmax >= deopt(x[i])) endif;
        constraint occurs(m) <-> exists (i in index_set(x)) (occurs(x[i]));
        constraint occurs(m) ->
            deopt(m) = min([if occurs(xi) then deopt(xi) else xmax endif | xi in x]);
    } in m;

/** @group optiontypes.float Return maximum of elements in \a x that are not absent, or
absent if all elements in \a x are absent. */
function var opt float: max(array[int] of var opt float: x) ::promise_total =
    let {
        var opt lb_array(x)..ub_array(x): m;
        var lb_array(x)..ub_array(x): xmin;
        constraint if lb(xmin) != -infinity then xmin = lb(xmin) else forall (i in index_set(x)) (xmin <= deopt(x[i])) endif;
        constraint occurs(m) <-> exists (i in index_set(x)) (occurs(x[i]));
        constraint occurs(m) ->
            deopt(m) = max([if occurs(xi) then deopt(xi) else xmin endif | xi in x]);
    } in m;

/** @group optiontypes.float Weak addition. Return sum of \a x and \a y if both
are present, otherwise return absent. */
function var opt float: '~+'(var opt float: x, var opt float: y) ::promise_total =
    let { 
    float: l = if lb(x)=-infinity \/ lb(y)=-infinity then -infinity else lb(x)+lb(y) endif;
    float: u = if ub(x)=infinity \/ ub(y)=infinity then infinity else ub(x)+ub(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)+deopt(y);
    } in result;

/** @group optiontypes.float Weak subtraction. Return difference of \a x and \a y if both
are present, otherwise return absent. */
function var opt float: '~-'(var opt float: x, var opt float: y) ::promise_total =
    let { 
    float: l = if lb(x)=-infinity \/ ub(y)=infinity then -infinity else lb(x)-ub(y) endif;
    float: u = if ub(x)=infinity \/ lb(y)=-infinity then infinity else ub(x)-lb(y) endif;
    var opt l..u: result; 
    constraint absent(x) \/ absent(y) -> result = <>;
    constraint absent(x) \/ absent(y) \/ result = deopt(x)-deopt(y);
    } in result;

/** @group optiontypes.float Weak multiplication. Return product of \a x and \a y if both
are present, otherwise return absent. */
function var opt float: '~*'(var opt float: x, var opt float: y) ::promise_total =
    if absent(x) \/ absent(y) then <>
    else deopt(x)*deopt(y) endif;

/** @group optiontypes.float Weak equality. True if either \a x or \a y are absent, or
present and equal.*/
function var bool: '~='(var opt float: x, var opt float: y) ::promise_total =
    absent(x) \/ absent(y) \/ deopt(x)=deopt(y);

/** @group optiontypes.float Return optional 0/1 float that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt float: bool2float(var opt bool: x) ::promise_total =
  let {
    var opt 0.0..1.0: xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=bool2float(deopt(x));
  } in xi;

/** @group optiontypes.int Return optional 0/1 integer that is absent iff \a x
  is absent, and 1 iff \a x occurs and is true. */
function var opt float: int2float(var opt int: x) ::promise_total =
  let {
    var opt int2float(lb(x))..int2float(ub(x)): xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=int2float(deopt(x));
  } in xi;


/** @group optiontypes.float True iff both \a x and \a y are absent or
  both are present and have the same value. */
predicate float_eq(var opt float: x, var opt float: y) =
     (absent(x) /\ absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)=deopt(y))::maybe_partial);

/** @group optiontypes.float True iff only one of \a x and \a y is absent or
  both are present and have different values. */
predicate float_ne(var opt float : x, var opt float : y) =
     (absent(x) != absent(y))
  \/ (occurs(x) /\ occurs(y) /\ (deopt(x)!=deopt(y))::maybe_partial);

/** @group optiontypes.float Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function var float: '+'(var opt float: x, var opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group optiontypes.float Optional addition. Return sum of \a x and \a y, with absent replaced by 0. */
function float: '+'(opt float: x, opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif + if occurs(y) then deopt(y) else 0 endif;

/** @group optiontypes.float Optional subtraction. Return difference of \a x and \a y, with absent replaced by 0. */
function var float: '-'(var opt float: x, var opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif - if occurs(y) then deopt(y) else 0 endif;

/** @group optiontypes.float Optional subtraction. Return difference of \a x and \a y, with absent replaced by 0. */
function float: '-'(opt float: x, opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 0 endif - if occurs(y) then deopt(y) else 0 endif;

/** @group optiontypes.float Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function var float: '*'(var opt float: x, var opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.float Optional multiplication. Return product of \a x and \a y, with absent replaced by 1. */
function float: '*'(opt float: x, opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif * if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.float Optional division. Return \a x div \a y, with absent replaced by 1. */
function var float: 'div'(var opt float: x, var opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif div if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.float Optional division. Return \a x div \a y, with absent replaced by 1. */
function float: 'div'(opt float: x, opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif div if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.float Optional modulo. Return \a x mod \a y, with absent replaced by 1. */
function var float: 'mod'(var opt float: x, var opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif mod if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.float Optional modulo. Return \a x mod \a y, with absent replaced by 1. */
function float: 'mod'(opt float: x, opt float: y) ::promise_total =
  if occurs(x) then deopt(x) else 1 endif mod if occurs(y) then deopt(y) else 1 endif;

/** @group optiontypes.float Return sum of non-absent elements of \a x. */
function var float: sum(array[int] of var opt float: x) =
  sum (i in index_set(x)) (let { var float: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 0 endif);

/** @group optiontypes.float Return sum of non-absent elements of \a x. */
function float: sum(array[int] of opt float: x) =
  sum (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 0 endif);

/** @group optiontypes.float Return product of non-absent elements of \a x. */
function var float: product(array[int] of var opt float: x) =
  product (i in index_set(x)) (let { var float: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 1 endif);

/** @group optiontypes.float Return product of non-absent elements of \a x. */
function float: product(array[int] of opt float: x) =
  product (i in index_set(x)) (if occurs(x[i]) then deopt(x[i]) else 1 endif);

/** @group optiontypes.float Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt float: element(var opt int: idx, array[int] of var float: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group optiontypes.float Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt float: element(var opt int: idx1, var opt int: idx2, array[int,int] of var float: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group optiontypes.float Return \a x[\a idx] */
function var opt float: element(var int: idx, array[int] of var opt float: x) =
  let {
    var opt float: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

/** @group optiontypes.float Return \a x[\a idx1, \a idx2] */
function var opt float: element(var int: idx1, var int: idx2, array[int,int] of var opt float: x) =
  let {
    var opt float: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

/** @group optiontypes.float Return absent if \a idx is absent, otherwise return \a x[\a idx] */
function var opt float: element(var opt int: idx, array[int] of var opt float: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

/** @group optiontypes.float Return absent if \a idx1 or \a idx2 is absent, otherwise return \a x[\a idx1, \a idx2] */
function var opt float: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt float: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

/** @group optiontypes.float Search annotation for optional float variables */
annotation float_search(array[int] of var opt float: x, ann: a1, ann: a2, ann: a3) =
  float_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2,a3);

/** @group optiontypes.float Search annotation for optional float variables */
annotation float_search(array[int] of var opt float: x, ann: a1, ann: a2) =
  float_search([if occurs(x[i]) then deopt(x[i]) else 0 endif | i in index_set(x)],a1,a2);

/* Internal function used to optimize over option type objective */

function var float: objective_deopt_(var opt float: x, bool: direction) =
  let {
    float: worst = if direction then lb(x)-1 else ub(x)+1 endif;
  } in if occurs(x) then deopt(x) else worst endif;

/***
 @groupdef options Compiler options
*/

/** @group options Whether to only generate domains that are contiguous ranges */
opt bool: mzn_opt_only_range_domains;

/** @group options Check whether to only generate domains that are contiguous ranges */

test mzn_check_only_range_domains() =
  if absent(mzn_opt_only_range_domains) then false
  else deopt(mzn_opt_only_range_domains) endif;

/** @group options If defined, this can be used to check that the MiniZinc compiler supports all the features used in the model. */
opt int: mzn_min_version_required;

constraint assert(absent(mzn_min_version_required) \/ deopt(mzn_min_version_required) <= mzn_compiler_version(), "This model requires MiniZinc version "++mzn_version_to_string(deopt(mzn_min_version_required))++" but you are running version "++mzn_version_to_string(mzn_compiler_version()));

predicate set_in(array[$T] of var int: X, set of int: s) = forall(x in array1d(X)) (x in s);
predicate int_eq(array[$T] of var int: X, int: s) = forall(x in array1d(X)) (x = s);
predicate float_eq(array[$T] of var int: X, float: s) = forall(x in array1d(X)) (x = s);
predicate int_le(array[$T] of var int: X, int: s) = forall(x in array1d(X)) (x <= s);
predicate int_le(int:s, array[$T] of var int: X) = forall(x in array1d(X)) (x >= s);
predicate float_le(array[$T] of var float: X, float: s) = forall(x in array1d(X)) (x <= s);
predicate float_le(float:s, array[$T] of var float: X) = forall(x in array1d(X)) (x >= s);

include "builtins.mzn";

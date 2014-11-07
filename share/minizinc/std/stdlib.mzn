%-----------------------------------------------------------------------------%
% MiniZinc standard library.
%-----------------------------------------------------------------------------%
% This file contains declarations of all functions, predicates and annotations
% available in the base MiniZinc language.

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

/** @group annotation.search Sequentially perform the searches specified in array \a s */
annotation seq_search(array[int] of ann: s);

annotation int_search(
    array[int] of var int: vars,
    ann: select,
    ann: choice,
    ann: explore,
);

annotation int_search_all(
    ann: select,
    ann: choice,
    ann: explore
);

annotation int_search_bd(
    ann: a,
    ann: b,
    ann: c,
    ann: d
);

annotation bool_search(
    array[int] of var bool: vars,
    ann: select,
    ann: choice,
    ann: explore
);

annotation float_search(
    array[int] of var float: vars,
    float: prec,
    ann: select,
    ann: choice,
    ann: explore
);

annotation set_search(
    array[int] of var set of int: vars,
    ann: select,
    ann: choice,
    ann: explore
);

annotation labelling_ff;

%-----------------------------------------------------------------------------%
%
% Variable selection annotations.
%

annotation input_order;
annotation first_fail;
annotation anti_first_fail;
annotation smallest;
annotation largest;
annotation occurrence;
annotation most_constrained;
annotation max_regret;
annotation dom_w_deg;
annotation impact;

%-----------------------------------------------------------------------------%
%
% Domain reduction strategies.
%

annotation indomain;
annotation indomain_min;
annotation indomain_max;
annotation indomain_middle;
annotation indomain_median;
annotation indomain_random;
annotation indomain_split;
annotation indomain_split_random;
annotation indomain_reverse_split;
annotation indomain_interval;
annotation outdomain_min;
annotation outdomain_median;
annotation outdomain_random;

%-----------------------------------------------------------------------------%
%
% Exploration strategies.
%

annotation complete;
annotation bbs(int: s);
annotation fail;
annotation lds(int: d);
annotation credit(int: c, ann: explore);
annotation dbs(int: d, ann: explore);
annotation restart_geometric(float: i, float: s, ann: search);
annotation limit(ann: measure, int: value, ann: search);
annotation time;

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

%-----------------------------------------------------------------------------%
%
% Option type predicates.
%

function $T: deopt(opt $T);

function var bool: occurs_internal(var opt bool: x) ::promise_total =
  let { var bool : b; } in b;
function var bool : deopt_internal(var opt bool : x) ::promise_total =
  let { var bool: y } in y;

function var opt bool: reverse_map(var bool: occ, var bool: d);
function opt bool: reverse_map(bool: occ, bool: d) ::promise_total =
  if occ then d else <> endif;

function var bool : occurs(var opt bool : x) ::promise_total =
  let {
    var bool : b = occurs_internal(x);
    var bool : dx = deopt_internal(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in b;
  
function var bool : deopt(var opt bool : x) ::promise_total =
  let {
    var bool : b = occurs_internal(x);
    var bool : dx = deopt_internal(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in dx;
  
predicate deopt(var bool: b) = b;
test occurs(var bool: b) = true;
test occurs(opt bool: x) ::promise_total;

predicate absent(var opt bool: b) = not occurs(b);
predicate absent(var bool: b) = false;
test absent(opt bool: x) ::promise_total = not occurs(x);

predicate bool_eq(var opt bool: b0, var opt bool: b1) =
     (absent(b0) /\ absent(b1))
  \/ (occurs(b0) /\ occurs(b1) /\ deopt(b0)=deopt(b1));

predicate bool_eq(var opt bool: b0, var bool: b1) =
  occurs(b0) /\ deopt(b0)=b1;

predicate bool_eq(var bool: b0, var opt bool: b1) =
  occurs(b1) /\ deopt(b1)=b0;

predicate forall (array[int] of var opt bool: x) =
  forall ([absent(x[i]) \/ deopt(x[i]) | i in index_set(x)]);

predicate exists (array[int] of var opt bool: x) =
  exists ([occurs(x[i]) /\ deopt(x[i]) | i in index_set(x)]);

function var bool: 'not'(var opt bool: x) = absent(x) \/ not deopt(x);

function bool: occurs(var int: x) ::promise_total = true;
function bool: occurs(int: x) ::promise_total = true;
test occurs(opt int: x) ::promise_total;
function var int: deopt(var int: x) ::promise_total = x;
function int: deopt(int: x) ::promise_total = x;

function bool: absent(var int: x) ::promise_total = false;
function bool: absent(opt int: x) ::promise_total = not occurs(x);
function var bool: absent(var opt int: x) ::promise_total = not occurs(x);

function var bool: occurs_internal(var opt int: x) ::promise_total =
  let { var bool : b; } in b;
function var int : deopt_internal(var opt int : x) ::promise_total =
  let { var lb(x)..ub(x): y } in y;

function var opt int: reverse_map(var bool: occ, var int: d);
function opt int: reverse_map(bool: occ, int: d) ::promise_total =
  if occ then d else <> endif;

function var bool : occurs(var opt int : x) ::promise_total =
  let {
    var bool : b = occurs_internal(x);
    var int  : dx = deopt_internal(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in b;
  
function var int : deopt(var opt int : x) ::promise_total =
  let {
    var bool : b = occurs_internal(x);
    var int  : dx = deopt_internal(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in dx;

function var opt int: bool2int(var opt bool: x) ::promise_total =
  let {
    var opt 0..1: xi;
    constraint absent(xi)=absent(x);
    constraint deopt(xi)=bool2int(deopt(x));
  } in xi;

predicate int_eq(var opt int : x, var opt int : y) =
     (absent(x) /\ absent(y))
  \/ (occurs(x) /\ occurs(y) /\ deopt(x)=deopt(y));

predicate int_ne(var opt int : x, var opt int : y) =
     (absent(x) != absent(y))
  \/ (occurs(x) /\ occurs(y) /\ deopt(x)!=deopt(y));

function var int: sum(array[int] of var opt int: x) =
  sum (i in index_set(x)) (let { var int: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 0 endif);

function var opt int: element(var opt int: idx, array[int] of var int: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

function var opt int: element(var opt int: idx1, var opt int: idx2, array[int,int] of var int: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

function var opt int: element(var int: idx, array[int] of var opt int: x) =
  let {
    var opt int: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

function var opt int: element(var int: idx1, var int: idx2, array[int,int] of var opt int: x) =
  let {
    var opt int: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

function var opt int: element(var opt int: idx, array[int] of var opt int: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

function var opt int: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt int: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;








function var opt bool: element(var opt int: idx, array[int] of var bool: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

function var opt bool: element(var opt int: idx1, var opt int: idx2, array[int,int] of var bool: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

function var opt bool: element(var int: idx, array[int] of var opt bool: x) =
  let {
    var opt bool: r;
    constraint occurs(r) = element(idx,array1d(index_set(x),[occurs(x[i]) | i in index_set(x)]));
    constraint deopt(r) = element(idx,array1d(index_set(x),[deopt(x[i]) | i in index_set(x)]));
  } in r;

function var opt bool: element(var int: idx1, var int: idx2, array[int,int] of var opt bool: x) =
  let {
    var opt bool: r;
    constraint occurs(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[occurs(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
    constraint deopt(r) = element(idx1,idx2,
      array2d(index_set_1of2(x),index_set_2of2(x),[deopt(x[i,j]) | i in index_set_1of2(x), j in index_set_2of2(x)]));
  } in r;

function var opt bool: element(var opt int: idx, array[int] of var opt bool: x) =
  if absent(idx) then <> else element(deopt(idx),x) endif;

function var opt bool: element(var opt int: idx1, var opt int: idx2, array[int,int] of var opt bool: x) =
  if absent(idx1) \/ absent(idx2) then <> else element(deopt(idx1),deopt(idx2),x) endif;

function var bool: '>'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
function var bool: '>='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
function var bool: '<'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
function var bool: '<='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);


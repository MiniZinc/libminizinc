/***
  @groupdef stdlib.builtins.compare Comparison Builtins

  These builtins implement comparison operations.
*/

/** @group stdlib.builtins.compare Return if \a x is less than \a y */
function     bool: '<'(    $T: x,    $T: y) ::mzn_internal_representation;
/** @group stdlib.builtins.compare Return if \a x is less than \a y */
function var bool: '<'(var $T: x,var $T: y) ::mzn_internal_representation;
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function var bool: '<'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function bool: '<'(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function var bool: '<'(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than the value of \a y. */
function bool: '<'(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) < deopt(y);

/** @group stdlib.builtins.compare Return if \a x is greater than \a y */
function     bool: '>'(    $T: x,    $T: y) ::mzn_internal_representation;
/** @group stdlib.builtins.compare Return if \a x is greater than \a y */
function var bool: '>'(var $T: x,var $T: y) ::mzn_internal_representation;
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function var bool: '>'(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function bool: '>'(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function var bool: '>'(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than the value of \a y. */
function bool: '>'(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) > deopt(y);

/** @group stdlib.builtins.compare Return if \a x is less than or equal to \a y */
function     bool: '<='(    $T: x,    $T: y) ::mzn_internal_representation;
/** @group stdlib.builtins.compare Return if \a x is less than or equal to \a y */
function var bool: '<='(var $T: x, var $T: y) ::mzn_internal_representation;
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function var bool: '<='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function bool: '<='(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function var bool: '<='(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is less than or equal to the value of \a y. */
function bool: '<='(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);

/** @group stdlib.builtins.compare Return if \a x is greater than or equal to \a y */
function     bool: '>='(    $T: x,    $T: y) ::mzn_internal_representation;
/** @group stdlib.builtins.compare Return if \a x is greater than or equal to \a y */
function var bool: '>='(var $T: x,var $T: y) ::mzn_internal_representation;
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function var bool: '>='(var opt int: x, var opt int: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function bool: '>='(opt int: x, opt int: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function var bool: '>='(var opt float: x, var opt float: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.builtins.compare Weak comparison: true iff either \a x or \a y is absent, or both
  occur and the value of \a x is greater than or equal to the value of \a y. */
function bool: '>='(opt float: x, opt float: y) = absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);

/** @group stdlib.builtins.compare Return if \a x is equal to \a y */
function     bool: '='(    $T: x,    $T: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.compare Return if \a x is equal to \a y */
function     bool: '='(opt $T: x, opt $T: y) ::promise_commutative =
  absent(x) /\ absent(y) \/ occurs(x) /\ occurs(y) /\ deopt(x) = deopt(y);
/** @group stdlib.builtins.compare Return if \a x is equal to \a y */
function var bool: '='(any $T: x,any $T: y) ::mzn_internal_representation ::promise_commutative;

/** @group stdlib.builtins.compare Weak equality. True if either \a x or \a y are absent, or
present and equal.*/
function var bool: '~='(var opt bool: x, var opt bool: y) ::promise_total ::promise_commutative =
    absent(x) \/ absent(y) \/ deopt(x)=deopt(y);

/** @group stdlib.builtins.compare Weak disequality. True if either \a x or \a y are absent, or
present and not equal.*/
function var bool: '~!='(var opt bool: x, var opt bool: y) ::promise_total ::promise_commutative =
    absent(x) \/ absent(y) \/ deopt(x)!=deopt(y);

/** @group stdlib.builtins.compare Weak equality. True if either \a x or \a y are absent, or
present and equal.*/
function var bool: '~='(var opt int: x, var opt int: y) ::promise_total ::promise_commutative =
    absent(x) \/ absent(y) \/ deopt(x)=deopt(y);

/** @group stdlib.builtins.compare Weak disequality. True if either \a x or \a y are absent, or
present and not equal.*/
function var bool: '~!='(var opt int: x, var opt int: y) ::promise_total ::promise_commutative =
    absent(x) \/ absent(y) \/ deopt(x)!=deopt(y);

/** @group stdlib.builtins.compare Weak equality. True if either \a x or \a y are absent, or
present and equal.*/
function var bool: '~='(var opt float: x, var opt float: y) ::promise_total ::promise_commutative =
    absent(x) \/ absent(y) \/ deopt(x)=deopt(y);

/** @group stdlib.builtins.compare Weak disequality. True if either \a x or \a y are absent, or
present and not equal.*/
function var bool: '~!='(var opt float: x, var opt float: y) ::promise_total ::promise_commutative =
    absent(x) \/ absent(y) \/ deopt(x)!=deopt(y);


/** @group stdlib.builtins.compare Return if \a x is not equal to \a y */
function     bool: '!='(    $T: x,     $T: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.compare Return if \a x is not equal to \a y */
function     bool: '!='(opt $T: x, opt $T: y) ::promise_commutative = not (x = y);
/** @group stdlib.builtins.compare Return if \a x is not equal to \a y */
function var bool: '!='(any $T: x, any $T: y) ::mzn_internal_representation ::promise_commutative;

% Special case comparison operators for integer variable and float constant
function var bool: '<='(var int: x, float: y) = (x <= floor(y));
function var bool: '>='(var int: x, float: y) = (x >= ceil(y));
function var bool: '<='(float: x, var int: y) = (y >= ceil(x));
function var bool: '>='(float: x, var int: y) = (y <= floor(x));

function var bool: '<'(var int: x, float: y) = (x <= ceil(y)-1);
function var bool: '>'(float: x, var int: y) = (y <= ceil(x)-1);
function var bool: '>'(var int: x, float: y) = (x >= floor(y)+1);
function var bool: '<'(float: x, var int: y) = (y >= floor(x)+1);

function var bool: '='(var int: x, float: y) =
  if ceil(y)=floor(y) then x=ceil(y) else false endif;
function var bool: '='(float: x, var int: y) =
  if ceil(x)=floor(x) then y=ceil(x) else false endif;

function var bool: '!='(var int: x, float: y) =
  if ceil(y)=floor(y) then x != ceil(y) else true endif;
function var bool: '!='(float: x, var int: y) =
  if ceil(x)=floor(x) then y != ceil(x) else true endif;

function bool: '<='(int: x, float: y) = (x <= floor(y));
function bool: '>='(int: x, float: y) = (x >= ceil(y));
function bool: '<='(float: x, int: y) = (y >= ceil(x));
function bool: '>='(float: x, int: y) = (y <= floor(x));

function bool: '<'(int: x, float: y) = (x <= ceil(y)-1);
function bool: '>'(float: x, int: y) = (y <= ceil(x)-1);
function bool: '>'(int: x, float: y) = (x >= floor(y)+1);
function bool: '<'(float: x, int: y) = (y >= floor(x)+1);

function bool: '='(int: x, float: y) =
  if ceil(y)=floor(y) then x=ceil(y) else false endif;
function bool: '='(float: x, int: y) =
  if ceil(x)=floor(x) then y=ceil(x) else false endif;

function bool: '!='(int: x, float: y) =
  if ceil(y)=floor(y) then x != ceil(y) else true endif;
function bool: '!='(float: x, int: y) =
  if ceil(x)=floor(x) then y != ceil(x) else true endif;

test lex_less(array[int] of $T: x,array[int] of $T: y) =
  if length(x)=1 /\ length(y)=1 then x[min(index_set(x))] < y[min(index_set(y))]
  elseif length(x)=0 then length(y)>0
  elseif length(y)=0 then false
  else let { int: lx = min(index_set(x)),
             int: ux = max(index_set(x)),
             int: ly = min(index_set(y)),
             int: uy = max(index_set(y)),
             int: size = min(ux - lx, uy - ly),
             int: first_not_equal = arg_max(i in 0..size)(x[lx+i] != y[ly+i]);
  } in if x[lx+first_not_equal-1]=y[ly+first_not_equal-1] then
    length(x) < length(y)
    else x[lx+first_not_equal-1]<y[ly+first_not_equal-1] endif
  endif;

test lex_lesseq(array[int] of $T: x,array[int] of $T: y) =
  if length(x)=1 /\ length(y)=1 then x[min(index_set(x))] <= y[min(index_set(y))]
  elseif length(x)=0 then true
  elseif length(y)=0 then false
  else let { int: lx = min(index_set(x)),
             int: ux = max(index_set(x)),
             int: ly = min(index_set(y)),
             int: uy = max(index_set(y)),
             int: size = min(ux - lx, uy - ly),
             int: first_not_equal = arg_max(i in 0..size)(x[lx+i] != y[ly+i]);
  } in if x[lx+first_not_equal-1]=y[ly+first_not_equal-1] then
    length(x) <= length(y)
    else x[lx+first_not_equal-1]<y[ly+first_not_equal-1] endif
  endif;


/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than array \a y */
function     bool: '<'(array[$U] of     $T: x,array[$U] of     $T: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than array \a y */
function var bool: '<'(array[$U] of var int: x,array[$U] of var int: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than array \a y */
function var bool: '<'(array[$U] of var bool: x,array[$U] of var bool: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than array \a y */
function var bool: '<'(array[$U] of var float: x,array[$U] of var float: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than array \a y */
function var bool: '<'(array[$U] of var set of int: x,array[$U] of var set of int: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than array \a y */
function     bool: '>'(array[$U] of     $T: x,array[$U] of     $T: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(y),array1d(x));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than array \a y */
function var bool: '>'(array[$U] of var int: x,array[$U] of var int: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(y),array1d(x));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than array \a y */
function var bool: '>'(array[$U] of var bool: x,array[$U] of var bool: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(y),array1d(x));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than array \a y */
function var bool: '>'(array[$U] of var float: x,array[$U] of var float: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(y),array1d(x));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than array \a y */
function var bool: '>'(array[$U] of var set of int: x,array[$U] of var set of int: y) =
  matching_index_sets(x, y) /\
  lex_less(array1d(y),array1d(x));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than or equal to array \a y */
function     bool: '<='(array[$U] of     $T: x,array[$U] of     $T: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than or equal to array \a y */
function var bool: '<='(array[$U] of var int: x,array[$U] of var int: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than or equal to array \a y */
function var bool: '<='(array[$U] of var bool: x,array[$U] of var bool: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than or equal to array \a y */
function var bool: '<='(array[$U] of var float: x,array[$U] of var float: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically smaller than or equal to array \a y */
function var bool: '<='(array[$U] of var set of int: x,array[$U] of var set of int: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(x),array1d(y));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than or equal to array \a y */
function     bool: '>='(array[$U] of     $T: x,array[$U] of     $T: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(y),array1d(x));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than or equal to array \a y */
function var bool: '>='(array[$U] of var int: x,array[$U] of var int: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(y),array1d(x));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than or equal to array \a y */
function var bool: '>='(array[$U] of var bool: x,array[$U] of var bool: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(y),array1d(x));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than or equal to array \a y */
function var bool: '>='(array[$U] of var float: x,array[$U] of var float: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(y),array1d(x));
/** @group stdlib.builtins.compare Return if array \a x is lexicographically greater than or equal to array \a y */
function var bool: '>='(array[$U] of var set of int: x,array[$U] of var set of int: y) =
  matching_index_sets(x, y) /\
  lex_lesseq(array1d(y),array1d(x));

/** @group stdlib.builtins.compare Return if array \a x is equal to array \a y */
function bool: '='(array[$U] of $T: x, array[$U] of $T: y) ::promise_commutative =
  matching_index_sets(x, y) /\
  let {
    any: xx = array1d(x);
    any: yy = array1d(y);
  } in forall (i in index_set(xx)) (xx[i]=yy[i]);

/** @group stdlib.builtins.compare Return if array \a x is equal to array \a y */
function bool: '='(array[$U] of opt $T: x, array[$U] of opt $T: y) ::promise_commutative =
  matching_index_sets(x, y) /\
  let {
    any: xx = array1d(x);
    any: yy = array1d(y);
  } in forall (i in index_set(xx)) (xx[i]=yy[i]);

/** @group stdlib.builtins.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$U] of var $T: x, array[$U] of var $T: y) ::promise_commutative =
  matching_index_sets(x, y) /\
    let {
        any: xx = array1d(x);
        any: yy = array1d(y);
    } in forall (i in index_set(xx)) (xx[i]=yy[i]);

/** @group stdlib.builtins.compare Return if array \a x is equal to array \a y */
function var bool: '='(array[$U] of  var opt $T: x,array[$U] of var opt $T: y) ::promise_commutative =
  matching_index_sets(x, y) /\
    let {
        any: xx = array1d(x);
        any: yy = array1d(y);
    } in forall (i in index_set(xx)) (xx[i]=yy[i]);

/** @group stdlib.builtins.compare Return if array \a x is not equal to array \a y */
function bool: '!='(array[$U] of $T: x, array[$U] of $T: y) ::promise_commutative =
  matching_index_sets(x, y) /\
  let {
    any: xx = array1d(x);
    any: yy = array1d(y);
  } in exists (i in index_set(xx)) (xx[i]!=yy[i]);

/** @group stdlib.builtins.compare Return if array \a x is not equal to array \a y */
function bool: '!='(array[$U] of opt $T: x, array[$U] of opt $T: y) ::promise_commutative =
  matching_index_sets(x, y) /\
  let {
    any: xx = array1d(x);
    any: yy = array1d(y);
  } in exists (i in index_set(xx)) (xx[i]!=yy[i]);

/** @group stdlib.builtins.compare Return if array \a x is not equal to array \a y */
function var bool: '!='(array[$U] of var $T: x, array[$U] of var $T: y) ::promise_commutative =
  matching_index_sets(x, y) /\
  let {
    any: xx = array1d(x);
    any: yy = array1d(y);
  } in exists (i in index_set(xx)) (xx[i]!=yy[i]);

/** @group stdlib.builtins.compare Return if array \a x is not equal to array \a y */
function var bool: '!='(array[$U] of var opt $T: x, array[$U] of var opt $T: y) ::promise_commutative =
  matching_index_sets(x, y) /\
  let {
    any: xx = array1d(x);
    any: yy = array1d(y);
  } in exists (i in index_set(xx)) (xx[i]!=yy[i]);

test matching_index_sets(array[$U] of any $T: x, array[$U] of any $T: y) =
  assert(index_sets_agree(x,y), "Index set mismatch. The index set of the left hand side value is " ++ show_index_sets(x) ++ ", but index set of the right hand side value is " ++ show_index_sets(y) ++ ". You may need to coerce the index sets using an array transformation.");

function string: show_index_sets(array[$U] of any $T: x);

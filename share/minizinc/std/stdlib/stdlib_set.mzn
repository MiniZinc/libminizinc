/***
  @groupdef stdlib.builtins.set Set operations

  These functions implement the basic operations on sets.
*/

/** @group stdlib.builtins.set Test if \a x is an element of the set \a y */
function bool: 'in'($$E: x, set of $$E: y) :: mzn_internal_representation;
/** @group stdlib.builtins.set \a x is an element of the set \a y */
function var bool: 'in'(var $$E: x, set of $$E: y);
/** @group stdlib.builtins.set \a x is an element of the set \a y */
function var bool: 'in'(var $$E: x, var set of $$E: y);
/** @group stdlib.builtins.set \a x is an element of the set \a y. True if \a x is absent. */
function var bool: 'in'(var opt $$E: x, set of $$E: y) = absent(x) \/ deopt(x) in y;
/** @group stdlib.builtins.set Test if \a x is an element of the set \a y. True if \a x is absent. */
function bool: 'in'(opt $$E: x, set of $$E: y) = absent(x) \/ deopt(x) in y;
/** @group stdlib.builtins.set \a x is an element of the set \a y. True if \a x is absent. */
function var bool: 'in'(var opt $$E: x, var set of $$E: y) = absent(x) \/ deopt(x) in y;

/** @group stdlib.builtins.set Test if \a x is an element of the set \a y */
function bool: 'in'(float: x, set of float: y) :: mzn_internal_representation;
/** @group stdlib.builtins.set Test if \a x is an element of the set \a y */
function var bool: 'in'(var float: x, set of float: y);
/** @group stdlib.builtins.set Test if \a x is an element of the set \a y. True if \a x is absent. */
function bool: 'in'(opt float: x, set of float: y) = absent(x) \/ deopt(x) in y;
/** @group stdlib.builtins.set \a x is an element of the set \a y. True if \a x is absent. */
function var bool: 'in'(var opt float: x, set of float: y) = absent(x) \/ deopt(x) in y;

/** @group stdlib.builtins.set Test if \a x is a subset of \a y */
function bool: 'subset'(set of $T: x, set of $T: y) :: mzn_internal_representation;
/** @group stdlib.builtins.set \a x is a subset of \a y */
function var bool: 'subset'(var set of $$E: x, var set of $$E: y);
/** @group stdlib.builtins.set Test if \a x is a superset of \a y */
function bool: 'superset'(set of $T: x, set of $T: y) :: mzn_internal_representation;
/** @group stdlib.builtins.set \a x is a superset of \a y */
function var bool: 'superset'(var set of $$E: x, var set of $$E: y);

/** @group stdlib.builtins.set Return the union of sets \a x and \a y */
function set of $T: 'union'(set of $T: x, set of $T: y)
  :: mzn_internal_representation
  :: promise_commutative;
/** @group stdlib.builtins.set Return the union of sets \a x and \a y */
function var set of $$T: 'union'(var set of $$T: x, var set of $$T: y)
  :: mzn_internal_representation
  :: promise_commutative;
/** @group stdlib.builtins.set Return the intersection of sets \a x and \a y */
function set of $T: 'intersect'(set of $T: x, set of $T: y)
  :: mzn_internal_representation
  :: promise_commutative;
/** @group stdlib.builtins.set Return the intersection of sets \a x and \a y */
function var set of $$T: 'intersect'(var set of $$T: x, var set of $$T: y)
  :: mzn_internal_representation
  :: promise_commutative;
/** @group stdlib.builtins.set Return the set difference of sets \(\a x \setminus \a y \) */
function set of $T: 'diff'(set of $T: x, set of $T: y) :: mzn_internal_representation;
/** @group stdlib.builtins.set Return the set difference of sets \(\a x \setminus \a y \) */
function var set of $$T: 'diff'(var set of $$T: x, var set of $$T: y)
   :: mzn_internal_representation;
/** @group stdlib.builtins.set Return the symmetric set difference of sets \a x and \a y */
function set of $T: 'symdiff'(set of $T: x, set of $T: y)
  :: mzn_internal_representation
  :: promise_commutative;
/** @group stdlib.builtins.set Return the symmetric set difference of sets \a x and \a y */
function var set of $$T: 'symdiff'(var set of $$T: x, var set of $$T: y)
  :: mzn_internal_representation
  :: promise_commutative;

/** @group stdlib.builtins.set Return the set \(\{\a a,\ldots,\a b\}\) */
function set of $$E: '..'($$E: a, $$E: b) :: mzn_internal_representation;
/** @group stdlib.builtins.set Return the set \(\{\a a,\ldots,\a b\}\) */
function set of float: '..'(float: a, float: b) :: mzn_internal_representation;
/** @group stdlib.builtins.set Return the set \(\{\a a,\ldots,\a b\}\) */
function set of bool: '..'(bool: a, bool: b) =
  if a then if b then {true} else {} endif elseif b then {false, true} else {false} endif;

/** @group stdlib.builtins.set Return the set \(\{\a a,\ldots,\a b-1\}\) */
function set of $$E: '..<'($$E: a, $$E: b) =
  if enum2int(a) >= enum2int(b) then {} else a..enum_prev(b) endif;
/** @group stdlib.builtins.set Return the set \(\{\a a+1,\ldots,\a b\}\) */
function set of $$E: '<..'($$E: a, $$E: b) =
  if enum2int(a) >= enum2int(b) then {} else enum_next(a)..b endif;
/** @group stdlib.builtins.set Return the set \(\{\a a+1,\ldots,\a b-1\}\) */
function set of $$E: '<..<'($$E: a, $$E: b) =
  if enum2int(b) - enum2int(a) <= 1 then {} else enum_next(a)..enum_prev(b) endif;

/** @group stdlib.builtins.set Return the set \(\{\a a,\ldots,\a b-1\}\) */
function set of $$E: '..o'($$E: a) = a..max(enum_of(a));
/** @group stdlib.builtins.set Return the set \(\{\a a,\ldots,\a b-1\}\) */
function set of $$E: '..<o'($$E: a) = a..enum_prev(max(enum_of(a)));
/** @group stdlib.builtins.set Return the set \(\{\a a+1,\ldots,\a b\}\) */
function set of $$E: '<..o'($$E: a) = (enum_next(a)..max(enum_of(a))) default {};
/** @group stdlib.builtins.set Return the set \(\{\a a+1,\ldots,\a b-1\}\) */
function set of $$E: '<..<o'($$E: a) = (enum_next(a)..enum_prev(max(enum_of(a)))) default {};

/** @group stdlib.builtins.set Return the set \(\{\a a,\ldots,\a b-1\}\) */
function set of $$E: 'o..'($$E: a) = min(enum_of(a))..a;
/** @group stdlib.builtins.set Return the set \(\{\a a,\ldots,\a b-1\}\) */
function set of $$E: 'o..<'($$E: a) = (min(enum_of(a))..enum_prev(a)) default {};
/** @group stdlib.builtins.set Return the set \(\{\a a+1,\ldots,\a b\}\) */
function set of $$E: 'o<..'($$E: a) = enum_next(min(enum_of(a)))..a;
/** @group stdlib.builtins.set Return the set \(\{\a a+1,\ldots,\a b-1\}\) */
function set of $$E: 'o<..<'($$E: a) = (enum_next(min(enum_of(a)))..enum_prev(a)) default {};

function set of int: '..'() = assert(false, "internal function '..'() called", {0});
function set of $$E: '..'(set of $$E: s) = min(s)..max(s);
function set of int: '<..<'() = assert(false, "internal function '<..<'() called", {0});
function set of $$E: '<..<'(set of $$E: s) = enum_next(min(s))..enum_prev(max(s));
function set of int: '..<'() = assert(false, "internal function '..<'() called", {0});
function set of $$E: '..<'(set of $$E: s) = min(s)..enum_prev(max(s));
function set of int: '<..'() = assert(false, "internal function '<..'() called", {0});
function set of $$E: '<..'(set of $$E: s) = enum_next(min(s))..max(s);

function var set of $$E: '..'(var $$E: a, var $$E: b) :: promise_total =
  let {
    var set of lb(a)..ub(b): s;
    constraint forall (i in ub(s)) (i in s <-> (a <= i /\ i <= b));
  } in s;

/** @group stdlib.builtins.set Return the cardinality of the set \a x */
function int: card(set of $T: x);
/** @group stdlib.builtins.set Return the cardinality of the set \a x */
function var int: card(var set of $$E: x) :: promise_total =
  let {
    var 0..(if has_ub_set(x) then card(ub(x)) else infinity endif): c;
    constraint set_card(enum2int(x), c);
  } in c;

/** @group stdlib.builtins.set Return the union of the sets in array \a x */
function set of $U: array_union(array [$T] of set of $U: x) :: promise_commutative;
/** @group stdlib.builtins.set Return the union of the sets in array \a x */
function var set of $$E: array_union(array [$T] of var set of $$E: x)
  :: promise_total
  :: promise_commutative =
  if length(x) = 0 then
    {}
  elseif length(x) = 1 then
    array1d(x)[1]
  else
    let {
      var set of ub_array(x): y :: is_defined_var;
      constraint fzn_array_set_union(enum2int(array1d(x)), enum2int(y)) :: defines_var(y);
    } in y
  endif;

/** @group stdlib.builtins.set Return the intersection of the sets in array \a x */
function set of $U: array_intersect(array [$T] of set of $U: x) :: promise_commutative;

/** @group stdlib.builtins.set Return the intersection of the sets in array \a x. Undefined for empty \a x. */
function var set of $$E: array_intersect(array [$T] of var set of $$E: x) :: promise_commutative =
  let { constraint length(x) > 0 } in array_intersect_t(x);

function var set of $$E: array_intersect_t(array [$T] of var set of $$E: x) :: promise_total =
  if length(x) = 0 then
    {}
  elseif length(x) = 1 then
    array1d(x)[1]
  else
    let {
      var set of ub_array(x): y :: is_defined_var;
      constraint fzn_array_set_intersect(enum2int(array1d(x)), enum2int(y)) :: defines_var(y);
    } in y
  endif;

/** @group stdlib.builtins.set Return the minimum of the set \a s */
function var $$E: min(var set of $$E: s);

/** @group stdlib.builtins.set Return the maximum of the set \a s */
function var $$E: max(var set of $$E: s);

/* Rewrite set membership on float variables to correct FlatZinc builtin */
predicate set_in(var float: x, set of float: S) = float_set_in(x, S);

/** @group stdlib.builtins.set Return a sorted list of the non-overlapping ranges in \a S */
function array [int] of $$E: set_to_ranges(set of $$E: S);
/** @group stdlib.builtins.set Return a sorted list of the non-overlapping ranges in \a S */
function array [int] of float: set_to_ranges(set of float: S);
/** @group stdlib.builtins.set Return a sorted list of the non-overlapping ranges in \a S */
function array [int] of bool: set_to_ranges(set of bool: S) =
  if S = {false} then [false, false] elseif S = {true} then [true, true] else [false, true] endif;

function array [int] of $$E: set_to_sparse_inverse(set of $$E: S);

/* set2iter will be used to redefine comprehensions over set decision variables
when it has a definition. */
function array [int] of var opt $$E: set2iter(var set of $$E: s) :: mzn_internal_representation;

/*
  Set operations on option types

  An absent operand is ignored: a relation with an absent operand is true, and an operation with
  an absent operand returns the other operand. If all operands of an operation are absent, the
  result is the identity of the operation if it has one ({} for union and symdiff), and absent
  otherwise (intersect, diff). In particular, e in s is equivalent to {e} subset s.
  Equality and disequality are strong: <> is only equal to <>.
*/

/** @group stdlib.builtins.set \a x is an element of the set \a y. True if \a y is absent. */
function var bool: 'in'(var $$E: x, var opt set of $$E: y) = absent(y) \/ x in deopt(y);
/** @group stdlib.builtins.set \a x is an element of the set \a y. True if \a x or \a y is absent. */
function var bool: 'in'(var opt $$E: x, var opt set of $$E: y) =
  absent(x) \/ absent(y) \/ deopt(x) in deopt(y);
/** @group stdlib.builtins.set Test if \a x is an element of the set \a y. True if \a y is absent. */
function bool: 'in'($$E: x, opt set of $$E: y) = absent(y) \/ x in deopt(y);
/** @group stdlib.builtins.set Test if \a x is an element of the set \a y. True if \a x or \a y is absent. */
function bool: 'in'(opt $$E: x, opt set of $$E: y) =
  absent(x) \/ absent(y) \/ deopt(x) in deopt(y);
/** @group stdlib.builtins.set Test if \a x is an element of the set \a y. True if \a y is absent. */
function bool: 'in'(float: x, opt set of float: y) = absent(y) \/ x in deopt(y);
/** @group stdlib.builtins.set Test if \a x is an element of the set \a y. True if \a x or \a y is absent. */
function bool: 'in'(opt float: x, opt set of float: y) =
  absent(x) \/ absent(y) \/ deopt(x) in deopt(y);

/** @group stdlib.builtins.set \a x is a subset of \a y. True if \a x or \a y is absent. */
function var bool: 'subset'(var opt set of $$E: x, var opt set of $$E: y) =
  absent(y) \/ (x default {}) subset deopt(y);
/** @group stdlib.builtins.set Test if \a x is a subset of \a y. True if \a x or \a y is absent. */
function bool: 'subset'(opt set of $T: x, opt set of $T: y) =
  absent(y) \/ (x default {}) subset deopt(y);
/** @group stdlib.builtins.set \a x is a superset of \a y. True if \a x or \a y is absent. */
function var bool: 'superset'(var opt set of $$E: x, var opt set of $$E: y) =
  absent(x) \/ deopt(x) superset (y default {});
/** @group stdlib.builtins.set Test if \a x is a superset of \a y. True if \a x or \a y is absent. */
function bool: 'superset'(opt set of $T: x, opt set of $T: y) =
  absent(x) \/ deopt(x) superset (y default {});

/** @group stdlib.builtins.set Set ordering. True if \a x or \a y is absent. */
function var bool: '<'(var opt set of $$E: x, var opt set of $$E: y) =
  absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.builtins.set Set ordering. True if \a x or \a y is absent. */
function bool: '<'(opt set of $T: x, opt set of $T: y) =
  absent(x) \/ absent(y) \/ deopt(x) < deopt(y);
/** @group stdlib.builtins.set Set ordering. True if \a x or \a y is absent. */
function var bool: '<='(var opt set of $$E: x, var opt set of $$E: y) =
  absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);
/** @group stdlib.builtins.set Set ordering. True if \a x or \a y is absent. */
function bool: '<='(opt set of $T: x, opt set of $T: y) =
  absent(x) \/ absent(y) \/ deopt(x) <= deopt(y);
/** @group stdlib.builtins.set Set ordering. True if \a x or \a y is absent. */
function var bool: '>'(var opt set of $$E: x, var opt set of $$E: y) =
  absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.builtins.set Set ordering. True if \a x or \a y is absent. */
function bool: '>'(opt set of $T: x, opt set of $T: y) =
  absent(x) \/ absent(y) \/ deopt(x) > deopt(y);
/** @group stdlib.builtins.set Set ordering. True if \a x or \a y is absent. */
function var bool: '>='(var opt set of $$E: x, var opt set of $$E: y) =
  absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);
/** @group stdlib.builtins.set Set ordering. True if \a x or \a y is absent. */
function bool: '>='(opt set of $T: x, opt set of $T: y) =
  absent(x) \/ absent(y) \/ deopt(x) >= deopt(y);

/** @group stdlib.builtins.set Return the union of sets \a x and \a y, ignoring absent sets.
  The union of two absent sets is the empty set. */
function var set of $$E: 'union'(var opt set of $$E: x, var opt set of $$E: y)
  :: promise_commutative = (x default {}) union (y default {});
/** @group stdlib.builtins.set Return the union of sets \a x and \a y, ignoring absent sets.
  The union of two absent sets is the empty set. */
function set of $T: 'union'(opt set of $T: x, opt set of $T: y) :: promise_commutative =
  (x default {}) union (y default {});

/** @group stdlib.builtins.set Return the symmetric set difference of sets \a x and \a y,
  ignoring absent sets. The symmetric difference of two absent sets is the empty set. */
function var set of $$E: 'symdiff'(var opt set of $$E: x, var opt set of $$E: y)
  :: promise_commutative = (x default {}) symdiff (y default {});
/** @group stdlib.builtins.set Return the symmetric set difference of sets \a x and \a y,
  ignoring absent sets. The symmetric difference of two absent sets is the empty set. */
function set of $T: 'symdiff'(opt set of $T: x, opt set of $T: y) :: promise_commutative =
  (x default {}) symdiff (y default {});

/** @group stdlib.builtins.set Return the intersection of sets \a x and \a y, ignoring absent sets.
  The intersection of two absent sets is absent. */
function var opt set of $$E: 'intersect'(var opt set of $$E: x, var opt set of $$E: y)
  :: promise_commutative =
  let {
    any: u = ub(x) union ub(y);
  } in mzn_opt_set(occurs(x) \/ occurs(y), (x default u) intersect (y default u), u);
/** @group stdlib.builtins.set Return the intersection of sets \a x and \a y, ignoring \a y if it is absent. */
function var set of $$E: 'intersect'(var set of $$E: x, var opt set of $$E: y) =
  x intersect (y default ub(x));
/** @group stdlib.builtins.set Return the intersection of sets \a x and \a y, ignoring \a x if it is absent. */
function var set of $$E: 'intersect'(var opt set of $$E: x, var set of $$E: y) =
  (x default ub(y)) intersect y;
/** @group stdlib.builtins.set Return the intersection of sets \a x and \a y, ignoring absent sets.
  The intersection of two absent sets is absent. */
function opt set of $T: 'intersect'(opt set of $T: x, opt set of $T: y) :: promise_commutative =
  if absent(x) then y elseif absent(y) then x else deopt(x) intersect deopt(y) endif;
/** @group stdlib.builtins.set Return the intersection of sets \a x and \a y, ignoring \a y if it is absent. */
function set of $T: 'intersect'(set of $T: x, opt set of $T: y) =
  if absent(y) then x else x intersect deopt(y) endif;
/** @group stdlib.builtins.set Return the intersection of sets \a x and \a y, ignoring \a x if it is absent. */
function set of $T: 'intersect'(opt set of $T: x, set of $T: y) =
  if absent(x) then y else deopt(x) intersect y endif;

/** @group stdlib.builtins.set Return the set difference of sets \(\a x \setminus \a y \), ignoring
  \a y if it is absent. Absent if \a x is absent. */
function var opt set of $$E: 'diff'(var opt set of $$E: x, var opt set of $$E: y) =
  mzn_opt_set(occurs(x), (x default {}) diff (y default {}), ub(x));
/** @group stdlib.builtins.set Return the set difference of sets \(\a x \setminus \a y \), ignoring
  \a y if it is absent. */
function var set of $$E: 'diff'(var set of $$E: x, var opt set of $$E: y) = x diff (y default {});
/** @group stdlib.builtins.set Return the set difference of sets \(\a x \setminus \a y \), ignoring
  \a y if it is absent. Absent if \a x is absent. */
function opt set of $T: 'diff'(opt set of $T: x, opt set of $T: y) =
  if absent(x) then <> else deopt(x) diff (y default {}) endif;
/** @group stdlib.builtins.set Return the set difference of sets \(\a x \setminus \a y \), ignoring
  \a y if it is absent. */
function set of $T: 'diff'(set of $T: x, opt set of $T: y) = x diff (y default {});

/** @group stdlib.builtins.set Return the cardinality of the set \a x. Absent if \a x is absent. */
function var opt int: card(var opt set of $$E: x) =
  if occurs(x) then card(deopt(x)) else <> endif;
/** @group stdlib.builtins.set Return the cardinality of the set \a x. Absent if \a x is absent. */
function opt int: card(opt set of $T: x) = if occurs(x) then card(deopt(x)) else <> endif;

/** @group stdlib.builtins.set Return the minimum of the set \a s. Absent if \a s is absent. */
function var opt $$E: min(var opt set of $$E: s) = if occurs(s) then min(deopt(s)) else <> endif;
/** @group stdlib.builtins.set Return the minimum of the set \a s. Absent if \a s is absent. */
function opt $$E: min(opt set of $$E: s) = if occurs(s) then min(deopt(s)) else <> endif;
/** @group stdlib.builtins.set Return the maximum of the set \a s. Absent if \a s is absent. */
function var opt $$E: max(var opt set of $$E: s) = if occurs(s) then max(deopt(s)) else <> endif;
/** @group stdlib.builtins.set Return the maximum of the set \a s. Absent if \a s is absent. */
function opt $$E: max(opt set of $$E: s) = if occurs(s) then max(deopt(s)) else <> endif;

/** @group stdlib.builtins.set Return the union of the sets in array \a x, ignoring absent sets */
function var set of $$E: array_union(array [$T] of var opt set of $$E: x) :: promise_commutative =
  array_union([xi default {} | xi in array1d(x)]);
/** @group stdlib.builtins.set Return the union of the sets in array \a x, ignoring absent sets */
function set of $U: array_union(array [$T] of opt set of $U: x) :: promise_commutative =
  array_union([xi default {} | xi in array1d(x)]);
/** @group stdlib.builtins.set Return the intersection of the sets in array \a x, ignoring absent
  sets. Absent if all sets are absent (or \a x is empty). */
function var opt set of $$E: array_intersect(array [$T] of var opt set of $$E: x)
  :: promise_commutative =
  if length(x) = 0 then
    <>
  else
    let {
      any: u = ub_array(x);
    } in mzn_opt_set(
      exists (xi in array1d(x)) (occurs(xi)),
      array_intersect([xi default u | xi in array1d(x)]),
      u,
    )
  endif;
/** @group stdlib.builtins.set Return the intersection of the sets in array \a x, ignoring absent
  sets. Absent if all sets are absent (or \a x is empty). */
function opt set of $U: array_intersect(array [$T] of opt set of $U: x) :: promise_commutative =
  let {
    any: xs = [deopt(xi) | xi in array1d(x) where occurs(xi)];
  } in if length(xs) = 0 then <> else array_intersect(xs) endif;

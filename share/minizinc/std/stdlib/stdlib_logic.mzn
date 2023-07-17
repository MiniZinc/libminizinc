/***
  @groupdef stdlib.builtins.logic Logical operations

  Logical operations are the standard operators of Boolean logic.
*/

/** @group stdlib.builtins.logic Return truth value of \a x &and; \a y */
function     bool: '/\'(    bool: x,     bool: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \a x &and; \a y with identity <> = True*/
function     bool: '/\'(opt bool: x, opt bool: y) ::promise_commutative = (absent(x) \/ deopt(x)) /\ (absent(y) \/ deopt(y));
/** @group stdlib.builtins.logic Return truth value of \a x &and; \a y */
function var bool: '/\'(var bool: x, var bool: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \a x &and; \a y with identity <> = True*/
function var bool: '/\'(var opt bool: x, var opt bool: y) ::promise_commutative = (absent(x) \/ deopt(x)) /\ (absent(y) \/ deopt(y));

/** @group stdlib.builtins.logic Return truth value of \a x &or; \a y */
function     bool: '\/'(    bool: x,     bool: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \a x &or; \a y with identity <> = False*/
function     bool: '\/'(opt bool: x, opt bool: y) ::promise_commutative = (occurs(x) /\ deopt(x)) \/ (occurs(y) /\ deopt(y));
/** @group stdlib.builtins.logic Return truth value of \a x &or; \a y */
function var bool: '\/'(var bool: x, var bool: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \a x &or; \a y with identity <> = False*/
function var bool: '\/'(var opt bool: x, var opt bool: y) ::promise_commutative = (occurs(x) /\ deopt(x)) \/ (occurs(y) /\ deopt(y));

/** @group stdlib.builtins.logic Return truth value of \a x implies \a y */
function     bool: '->'(    bool: x,     bool: y) ::mzn_internal_representation;
/** @group stdlib.builtins.logic Return truth value of \a x implies \a y */
function     bool: '->'(opt bool: x, opt bool: y) =
  absent(x) \/ absent(y) \/ (deopt(x) -> deopt(y));
/** @group stdlib.builtins.logic Return truth value of \a x implies \a y */
function var bool: '->'(var bool: x, var bool: y);
/** @group stdlib.builtins.logic Return truth value of \a x implies \a y */
function var bool: '->'(var opt bool: x, var opt bool: y) =
  absent(x) \/ absent(y) \/ (deopt(x) -> deopt(y));
/** @group stdlib.builtins.logic Return truth value of \a y implies \a x */
function     bool: '<-'(    bool: x,     bool: y) ::mzn_internal_representation;
/** @group stdlib.builtins.logic Return truth value of \a y implies \a x */
function     bool: '<-'(opt bool: x, opt bool: y) =
  absent(x) \/ absent(y) \/ (deopt(x) <- deopt(y));
/** @group stdlib.builtins.logic Return truth value of \a y implies \a x */
function var bool: '<-'(var bool: x, var bool: y);
/** @group stdlib.builtins.logic Return truth value of \a y implies \a x */
function var bool: '<-'(var opt bool: x, var opt bool: y) =
  absent(x) \/ absent(y) \/ (deopt(x) <- deopt(y));
/** @group stdlib.builtins.logic Return truth value of \a x if-and-only-if \a y */
function     bool: '<->'(    bool: x,     bool: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \a x if-and-only-if \a y */
function     bool: '<->'(opt bool: x, opt bool: y) ::promise_commutative =
  absent(x) \/ absent(y) \/ (deopt(x) <-> deopt(y));
/** @group stdlib.builtins.logic Return truth value of \a x if-and-only-if \a y */
function var bool: '<->'(var bool: x, var bool: y) ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \a x if-and-only-if \a y */
function var bool: '<->'(var opt bool: x, var opt bool: y) ::promise_commutative =
  absent(x) \/ absent(y) \/ (deopt(x) <-> deopt(y));
/** @group stdlib.builtins.logic Return truth value of \a x xor \a y */
function     bool: 'xor'(    bool: x,     bool: y) ::mzn_internal_representation ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \a x xor \a y */
function     bool: 'xor'(opt bool: x, opt bool: y) ::promise_commutative =
  absent(x) \/ absent(y) \/ (deopt(x) xor deopt(y));
/** @group stdlib.builtins.logic Return truth value of \a x xor \a y */
function var bool: 'xor'(var bool: x, var bool: y) ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \a x xor \a y */
function var bool: 'xor'(var opt bool: x, var opt bool: y) ::promise_commutative =
  absent(x) \/ absent(y) \/ (deopt(x) xor deopt(y));
/** @group stdlib.builtins.logic Return truth value of the negation of \a x */
function     bool: 'not'(    bool: x) ::mzn_internal_representation;
/** @group stdlib.builtins.logic Negation of \a x if it occurs, otherwise absent */
function opt bool: 'not'(opt bool: x) = if absent(x) then <> else not deopt(x) endif;
/** @group stdlib.builtins.logic Return truth value of the negation of \a x */
function var bool: 'not'(var bool: x);
/** @group stdlib.builtins.logic Negation of \a x if it occurs, otherwise absent */
function var opt bool: 'not'(var opt bool: x) = if absent(x) then <> else not deopt(x) endif;

/** @group stdlib.builtins.logic Return truth value of \(\bigwedge_i \a x[i]\) */
function     bool: forall(array[$T] of     bool: x) ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \(\bigwedge_i \a x[i]\) */
function var bool: forall(array[$T] of var bool: x) ::promise_commutative;
/** @group stdlib.builtins.logic True iff for any \p i, \a x[i] is absent or true */
predicate forall (array[int] of var opt bool: x) ::promise_commutative =
  forall ([absent(x[i]) \/ deopt(x[i]) | i in index_set(x)]);

/** @group stdlib.builtins.logic Return truth value of \(\bigvee_i \a x[i]\) */
function     bool: exists(array[$T] of     bool: x) ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \(\bigvee_i \a x[i]\) */
function var bool: exists(array[$T] of var bool: x) ::promise_commutative;
/** @group stdlib.builtins.logic True iff for at least one \p i, \a x[i] occurs and is true */
predicate exists (array[int] of var opt bool: x) =
  exists ([occurs(x[i]) /\ deopt(x[i]) | i in index_set(x)]);

/** @group stdlib.builtins.logic Return truth value of \(\oplus_i \a x[i]\) */
function     bool: xorall(array[$T] of     bool: x) ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \(\oplus_i \a x[i]\) */
function     bool: xorall(array[$T] of opt bool: x) ::promise_commutative =
  exists(y in array1d(x))(absent(y)) \/ xorall(y in array1d(x))(deopt(y));
/** @group stdlib.builtins.logic Return truth value of \(\oplus_i \a x[i]\) */
function var bool: xorall(array[$T] of var bool: x) ::promise_commutative =
  array_bool_xor(array1d(x));
/** @group stdlib.builtins.logic Return truth value of \(\oplus_i \a x[i]\) */
function var bool: xorall(array[$T] of var opt bool: x) ::promise_commutative =
  exists(y in array1d(x))(absent(y)) \/ xorall(y in array1d(x))(deopt(y));
/** @group stdlib.builtins.logic Return truth value of \(\text{true}\oplus (\oplus_i \a x[i])\) */
function     bool: iffall(array[$T] of     bool: x) ::promise_commutative;
/** @group stdlib.builtins.logic Return truth value of \(\text{true}\oplus (\oplus_i \a x[i])\) */
function     bool: iffall(array[$T] of opt bool: x) ::promise_commutative =
  exists(y in array1d(x))(absent(y)) \/ iffall(y in array1d(x))(deopt(y));
/** @group stdlib.builtins.logic Return truth value of \(\text{true}\oplus (\oplus_i \a x[i])\) */
function var bool: iffall(array[$T] of var bool: x) ::promise_commutative =
  array_bool_xor(array1d(x)++[true]);
/** @group stdlib.builtins.logic Return truth value of \(\text{true}\oplus (\oplus_i \a x[i])\) */
function var bool: iffall(array[$T] of var opt bool: x) ::promise_commutative =
  exists(y in array1d(x))(absent(y)) \/ iffall(y in array1d(x))(deopt(y));
/** @group stdlib.builtins.logic Return truth value of \((\bigvee_i \a x[i]) \lor (\bigvee_j \lnot \a y[j])\) */
function     bool: clause(array[$T] of     bool: x, array[$T] of     bool: y);
/** @group stdlib.builtins.logic Return truth value of \((\bigvee_i \a x[i]) \lor (\bigvee_j \lnot \a y[j])\) */
function     bool: clause(array[$T] of opt bool: x, array[$T] of opt bool: y) =
  exists(z in array1d(x))(absent(z)) \/ exists(z in array1d(y))(absent(z)) \/
  clause([deopt(z) | z in array1d(x)], [deopt(z) | z in array1d(y)]);
/** @group stdlib.builtins.logic Return truth value of \((\bigvee_i \a x[i]) \lor (\bigvee_j \lnot \a y[j])\) */
function var bool: clause(array[$T] of var bool: x, array[$T] of var bool: y);
/** @group stdlib.builtins.logic Return truth value of \((\bigvee_i \a x[i]) \lor (\bigvee_j \lnot \a y[j])\) */
function var bool: clause(array[$T] of var opt bool: x, array[$T] of var opt bool: y) =
  exists(z in array1d(x))(absent(z)) \/ exists(z in array1d(y))(absent(z)) \/
  clause([deopt(z) | z in array1d(x)], [deopt(z) | z in array1d(y)]);

/** @group stdlib.builtins.logic Return negation of \a b */
function var bool: bool_not(var bool: b);


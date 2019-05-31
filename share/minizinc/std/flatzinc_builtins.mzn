%-----------------------------------------------------------------------------%
%
% FlatZinc builtins
%
%  This section contains declarations for the standard FlatZinc builtins.
%  They can be redefined by providing a custom redefinitions.mzn in the
%  solver globals library.
%

/***
  @groupdef flatzinc FlatZinc builtins
  
  These are the standard constraints that need to be supported by FlatZinc
  solvers (or redefined in the redefinitions.mzn file).
*/

/***
  @groupdef flatzinc.int Integer FlatZinc builtins
*/

/** @group flatzinc.int Constrains \a b to be the absolute value of \a a */
predicate int_abs(var int: a, var int: b);
/** @group flatzinc.int Constrains \a a to be equal to \a b */
predicate int_eq(var int: a, var int: b);
/** @group flatzinc.int Constrains (\a a=\a b) \( \leftrightarrow \) \a r */
predicate int_eq_reif(var int: a, var int: b, var bool: r);
/** @group flatzinc.int Constrains \a a to be less than or equal to \a b */
predicate int_le(var int: a, var int: b);
/** @group flatzinc.int Constrains (\a a ≤ \a b) \( \leftrightarrow \) \a r */
predicate int_le_reif(var int: a, var int: b, var bool: r);
/** @group flatzinc.int Constrains \( \a c = \sum_i \a as[i]*\a bs[i] \) */
predicate int_lin_eq(array[int] of int: as, array[int] of var int: bs, int: c);
/** @group flatzinc.int Constrains \( \a r \leftrightarrow (\a c = \sum_i \a as[i]*\a bs[i]) \) */
predicate int_lin_eq_reif(array[int] of int: as, array[int] of var int: bs,int: c, var bool: r);
/** @group flatzinc.int Constrains \( \a c \neq \sum_i \a as[i]*\a bs[i] \) */
predicate int_lin_ne(array[int] of int: as, array[int] of var int: bs, int: c);
/** @group flatzinc.int Constrains \( \a r \leftrightarrow (\a c \neq \sum_i \a as[i]*\a bs[i]) \) */
predicate int_lin_ne_reif(array[int] of int: as, array[int] of var int: bs,int: c, var bool: r);

/** @group flatzinc.int Constrains \( \sum \) \a as[\p i]*\a bs[\p i] ≤ \a c */
predicate int_lin_le(array[int] of int: as, array[int] of var int: bs, int: c);
/** @group flatzinc.int Constrains \a r \( \leftrightarrow \) (\( \sum \) \a as[\p i]*\a bs[\p i] ≤ \a c) */
predicate int_lin_le_reif(array[int] of int: as, array[int] of var int: bs,int: c, var bool: r);
/** @group flatzinc.int Constrains \a a ≠ \a b */
predicate int_ne(var int: a, var int: b);
/** @group flatzinc.int \a r \( \leftrightarrow \) (\a a ≠ \a b) */
predicate int_ne_reif(var int: a, var int: b, var bool: r);
/** @group flatzinc.int Constrains \a a + \a b = \a c */
predicate int_plus(var int: a, var int: b, var int: c);
/** @group flatzinc.int Constrains \a a / \a b = \a c */
predicate int_div(var int: a, var int: b, var int: c);
/** @group flatzinc.int Constrains \a a < \a b */
predicate int_lt(var int: a, var int: b);
/** @group flatzinc.int Constrains \a r \( \leftrightarrow \) (\a a < \a b) */
predicate int_lt_reif(var int: a, var int: b, var bool: r);
/** @group flatzinc.int Constrains max(\a a, \a b) = \a c */
predicate int_max(var int: a, var int: b, var int: c);
/** @group flatzinc.int Constrains min(\a a, \a b) = \a c */
predicate int_min(var int: a, var int: b, var int: c);
/** @group flatzinc.int Constrains \a a % \a b = \a c */
predicate int_mod(var int: a, var int: b, var int: c);
/** @group flatzinc.int Constrains \a a * \a b = \a c */
predicate int_times(var int: a, var int: b, var int: c);
/** @group flatzinc.int Constrains \a z = \(\a x ^ {\a y}\) */
predicate int_pow(var int: x, var int: y, var int: z);

/***
  @groupdef flatzinc.bool Bool FlatZinc builtins
*/

/** @group flatzinc.bool Constrains \( \a b \in \{0,1\} \) and \( \a a \leftrightarrow \a b=1 \) */
predicate bool2int(var bool: a, var int: b);
/** @group flatzinc.bool Constrains \( \a r \leftrightarrow \a a \land \a b \) */
predicate bool_and(var bool: a, var bool: b, var bool: r);
/** @group flatzinc.bool Constrains \( \bigvee_i \a as[i] \lor \bigvee_j \lnot \a bs[j] \) */
predicate bool_clause(array[int] of var bool: as, array[int] of var bool: bs);

/** @group flatzinc.bool Constrains \a a = \a b */
predicate bool_eq(var bool: a, var bool: b);
/** @group flatzinc.bool Constrains \a r \( \leftrightarrow \) (\a a = \a b) */
predicate bool_eq_reif(var bool: a, var bool: b, var bool: r);
/** @group flatzinc.bool Constrains \a a ≤ \a b */
predicate bool_le(var bool: a, var bool: b);
/** @group flatzinc.bool Constrains \a r \( \leftrightarrow \) (\a a ≤ \a b) */
predicate bool_le_reif(var bool: a, var bool: b, var bool: r);
/** @group flatzinc.bool Constrains \( \a c = \sum_i \a as[i]*\a bs[i] \) */
predicate bool_lin_eq(array[int] of int: as, array[int] of var bool: bs, var int: c);
/** @group flatzinc.bool Constrains \( \a c \leq \sum_i \a as[i]*\a bs[i] \) */
predicate bool_lin_le(array[int] of int: as, array[int] of var bool: bs, int: c);
/** @group flatzinc.bool Constrains \a a < \a b */
predicate bool_lt(var bool: a, var bool: b);
/** @group flatzinc.bool Constrains \a r \( \leftrightarrow \) (\a a < \a b) */
predicate bool_lt_reif(var bool: a, var bool: b, var bool: r);
/** @group flatzinc.bool Constrains \a a ≠ \a b */
predicate bool_not(var bool: a, var bool: b);
/** @group flatzinc.bool Constrains \( \a r \leftrightarrow \a a \lor \a b \) */
predicate bool_or(var bool: a, var bool: b, var bool: r);
/** @group flatzinc.bool Constrains \( \a r \leftrightarrow \a a \oplus \a b \) */
predicate bool_xor(var bool: a, var bool: b, var bool: r);
/** @group flatzinc.bool Constrains \a a \( \oplus \) \a b */
predicate bool_xor(var bool: a, var bool: b);

/***
  @groupdef flatzinc.set Set FlatZinc builtins
*/

/** @group flatzinc.set Constrains \a x \( \in \) \a S */
predicate set_in(var int: x, set of int: S);
/** @group flatzinc.set Constrains \a x \( \in \) \a S */
predicate set_in(var int: x, var set of int: S);
/** @group flatzinc.set Constrains \a x = |\a S| */
predicate set_card(var set of int: S, var int: x);
/** @group flatzinc.set Constrains \( \a r \leftrightarrow (\a x \in \a S) \) */
predicate set_in_reif(var int: x, set of int: S, var bool: r);
/** @group flatzinc.set Constrains \( \a r \leftrightarrow (\a x \in \a S) \) */
predicate set_in_reif(var int: x, var set of int: S, var bool: r);
/** @group flatzinc.set Constrains \a x \( \subseteq \) \a y */
predicate set_subset(var set of int: x, var set of int: y);
/** @group flatzinc.set Constrains \a x \( \supseteq \) \a y */
predicate set_superset(var set of int: x, var set of int: y);
/** @group flatzinc.set Constrains \( \a r \leftrightarrow (\a x \subseteq \a y) \) */
predicate set_subset_reif(var set of int: x, var set of int: y, var bool: r);
/** @group flatzinc.set Constrains \a x ≤ \a y (lexicographic order) */
predicate set_le(var set of int: x, var set of int: y);
/** @group flatzinc.set Constrains \a x < \a y (lexicographic order) */
predicate set_lt(var set of int: x, var set of int: y);
/** @group flatzinc.set Constrains \a x = \a y */
predicate set_eq(var set of int: x, var set of int: y);
/** @group flatzinc.set Constrains \a r \( \leftrightarrow \) (\a x = \a y) */
predicate set_eq_reif(var set of int: x, var set of int: y, var bool: r);
/** @group flatzinc.set Constrains \a x ≠ \a y */
predicate set_ne(var set of int: x, var set of int: y);
/** @group flatzinc.set Constrains \a r \( \leftrightarrow \) (\a x ≠ \a y) */
predicate set_ne_reif(var set of int: x, var set of int: y, var bool: r);
/** @group flatzinc.set Constrains \a r = \a x \( \cap \) \a y */
predicate set_intersect(var set of int: x, var set of int: y, var set of int: r);
/** @group flatzinc.set Constrains \a r = \a x \( \cup \) \a y */
predicate set_union(var set of int: x, var set of int: y, var set of int: r);
/** @group flatzinc.set Constrains \a r = \a x \( \setminus \) \a y */
predicate set_diff(var set of int: x, var set of int: y, var set of int: r);
/** @group flatzinc.set Constrains \a r to be the symmetric difference of \a x and \a y */
predicate set_symdiff(var set of int: x, var set of int: y, var set of int: r);

/***
  @groupdef flatzinc.float Float FlatZinc builtins
*/

/** @group flatzinc.float Constrains \a b to be the absolute value of \a a */
predicate float_abs(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = acos(\a a) */
predicate float_acos(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = acosh(\a a) */
predicate float_acosh(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = asin(\a a) */
predicate float_asin(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = asinh(\a a) */
predicate float_asinh(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = atan(\a a) */
predicate float_atan(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = atanh(\a a) */
predicate float_atanh(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = cos(\a a) */
predicate float_cos(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = cosh(\a a) */
predicate float_cosh(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = exp(\a a) */
predicate float_exp(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = ln(\a a) */
predicate float_ln(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = log<sub>10</sub>(\a a) */
predicate float_log10(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = log<sub>2</sub>(\a a) */
predicate float_log2(var float: a, var float: b);
/** @group flatzinc.float Constrains \(\a b = \sqrt{\a a}\) */
predicate float_sqrt(var float: a, var float: b);
/** @group flatzinc.float Constrains \a z = \(\a x ^ {\a y}\) */
predicate float_pow(var float: x, var float: y, var float: z);
/** @group flatzinc.float Constrains \a b = sin(\a a) */
predicate float_sin(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = sinh(\a a) */
predicate float_sinh(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = tan(\a a) */
predicate float_tan(var float: a, var float: b);
/** @group flatzinc.float Constrains \a b = tanh(\a a) */
predicate float_tanh(var float: a, var float: b);

/** @group flatzinc.float Constrains \a a = \a b */
predicate float_eq(var float: a, var float: b);
/** @group flatzinc.float Constrains \a r \( \leftrightarrow \) (\a a = \a b) */
predicate float_eq_reif(var float: a, var float: b, var bool: r);
/** @group flatzinc.float Constrains \a a ≤ \a b */
predicate float_le(var float: a, var float: b);
/** @group flatzinc.float Constrains \a r \( \leftrightarrow \) (\a a ≤ \a b) */
predicate float_le_reif(var float: a, var float: b, var bool: r);
/** @group flatzinc.float Constrains \a a < \a b */
predicate float_lt(var float: a, var float: b);
/** @group flatzinc.float Constrains \a r \( \leftrightarrow \) (\a a < \a b) */
predicate float_lt_reif(var float: a, var float: b, var bool: r);
/** @group flatzinc.float Constrains \a a ≠ \a b */
predicate float_ne(var float: a, var float: b);
/** @group flatzinc.float Constrains \a r \( \leftrightarrow \) (\a a ≠ \a b) */
predicate float_ne_reif(var float: a, var float: b, var bool: r);
/** @group flatzinc.float Constrains \( \a a \in\ [ \a b, \a c ] \) */
predicate float_in(var float: a, float: b, float: c);
/** @group flatzinc.float Constrains \a r \( \leftrightarrow \) \( \a a \in\ [ \a b, \a c ] \) */
predicate float_in_reif(var float: a, float: b, float: c, var bool: r);
/** @group flatzinc.float Constrains the domain of \a x using the values in \a as, using each
  pair of values \a as[2*\p i-1]..\a as[2*\p i] for \p i in 1..\p n/2 as a possible range
 */
predicate float_dom(var float: x, array[int] of float: as);

/** @group flatzinc.float Constrains \( \a c = \sum_i \a as[i]*\a bs[i] \) */
predicate float_lin_eq(array[int] of float: as, array[int] of var float: bs, float: c);
/** @group flatzinc.float Constrains \( \a r \leftrightarrow (\a c = \sum_i \a as[i]*\a bs[i]) \) */
predicate float_lin_eq_reif(array[int] of float: as, array[int] of var float: bs, float: c, var bool: r);
/** @group flatzinc.float Constrains \( \a c \leq \sum_i \a as[i]*\a bs[i] \) */
predicate float_lin_le(array[int] of float: as, array[int] of var float: bs, float: c);
/** @group flatzinc.float Constrains \( \a r \leftrightarrow (\a c \leq \sum_i \a as[i]*\a bs[i]) \) */
predicate float_lin_le_reif(array[int] of float: as, array[int] of var float: bs, float: c, var bool: r);
/** @group flatzinc.float Constrains \( \a c < \sum_i \a as[i]*\a bs[i] \) */
predicate float_lin_lt(array[int] of float: as, array[int] of var float: bs, float: c);
/** @group flatzinc.float Constrains \( \a r \leftrightarrow (\a c < \sum_i \a as[i]*\a bs[i]) \) */
predicate float_lin_lt_reif(array[int] of float: as, array[int] of var float: bs, float: c, var bool: r);
/** @group flatzinc.float Constrains \( \a c \neq \sum_i \a as[i]*\a bs[i] \) */
predicate float_lin_ne(array[int] of float: as, array[int] of var float: bs, float: c);
/** @group flatzinc.float Constrains \( \a r \leftrightarrow (\a c \neq \sum_i \a as[i]*\a bs[i]) \) */
predicate float_lin_ne_reif(array[int] of float: as, array[int] of var float: bs, float: c, var bool: r);

/** @group flatzinc.float Constrains max(\a a, \a b) = \a c */
predicate float_max(var float: a, var float: b, var float: c);
/** @group flatzinc.float Constrains min(\a a, \a b) = \a c */
predicate float_min(var float: a, var float: b, var float: c);
/** @group flatzinc.float Constrains \a a + \a b = \a c */
predicate float_plus(var float: a, var float: b, var float: c);
/** @group flatzinc.float Constrains \a a / \a b = \a c */
predicate float_div(var float: a, var float: b, var float: c);
/** @group flatzinc.float Constrains \a a * \a b = \a c */
predicate float_times(var float: a, var float: b, var float: c);

/** @group flatzinc.float Constrains \a y=\a x */
predicate int2float(var int: x, var float: y);


% Array constraints

/** @group flatzinc.bool Constrains \( \a r \leftrightarrow \bigwedge_i \a as[i]\) */
predicate array_bool_and(array[int] of var bool: as, var bool: r);
/** @group flatzinc.bool Constrains \( \a r \leftrightarrow \bigvee_i \a as[i]\) */
predicate array_bool_or(array[int] of var bool: as, var bool: r);
/** @group flatzinc.bool Constrains \( \a r \leftrightarrow \oplus_i\ \a as[i]\) */
predicate array_bool_xor(array[int] of var bool: as);
/** @group flatzinc.bool Constrains \a as[\a b] = \a c */
predicate array_bool_element(var int: b, array[int] of bool: as, var bool: c);
/** @group flatzinc.int Constrains \a as[\a b] = \a c */
predicate array_int_element(var int: b, array[int] of int: as, var int: c);
/** @group flatzinc.float Constrains \a as[\a b] = \a c */
predicate array_float_element(var int: b, array[int] of float: as, var float: c);
/** @group flatzinc.set Constrains \a as[\a b] = \a c */
predicate array_set_element(var int: b, array[int] of set of int: as, var set of int: c);
/** @group flatzinc.bool Constrains \a as[\a b] = \a c */
predicate array_var_bool_element(var int: b, array[int] of var bool: as, var bool: c);
/** @group flatzinc.int Constrains \a as[\a b] = \a c */
predicate array_var_int_element(var int: b, array[int] of var int: as, var int: c);
/** @group flatzinc.float Constrains \a as[\a b] = \a c */
predicate array_var_float_element(var int: b, array[int] of var float: as, var float: c);
/** @group flatzinc.set Constrains \a as[\a b] = \a c */
predicate array_var_set_element(var int: b, array[int] of var set of int: as, var set of int: c);
/** @group flatzinc.int Constrains \a m to be the maximum value of the (non-empty)
  array \a x */
predicate array_int_maximum(var int: m, array[int] of var int: x);
/** @group flatzinc.float Constrains \a m to be the maximum value of the (non-empty)
  array \a x */
predicate array_float_maximum(var int: m, array[int] of var int: x);
/** @group flatzinc.int Constrains \a m to be the minimum value of the (non-empty)
  array \a x */
predicate array_int_minimum(var int: m, array[int] of var int: x);
/** @group flatzinc.float Constrains \a m to be the minimum value of the (non-empty)
  array \a x */
predicate array_float_minimum(var int: m, array[int] of var int: x);

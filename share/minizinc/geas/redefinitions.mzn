%
%  Main authors:
%     Guido Tack <tack@gecode.org>
%
%  Copyright:
%     Guido Tack, 2007
%
%  Last modified:
%     $Date: 2009-11-09 22:08:26 +0100 (Mon, 09 Nov 2009) $ by $Author: tack $
%     $Revision: 10067 $
%
%  This file is part of Gecode, the generic constraint
%  development environment:
%     http://www.gecode.org
%
%  Permission is hereby granted, free of charge, to any person obtaining
%  a copy of this software and associated documentation files (the
%  "Software"), to deal in the Software without restriction, including
%  without limitation the rights to use, copy, modify, merge, publish,
%  distribute, sublicense, and/or sell copies of the Software, and to
%  permit persons to whom the Software is furnished to do so, subject to
%  the following conditions:
%
%  The above copyright notice and this permission notice shall be
%  included in all copies or substantial portions of the Software.
%
%  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
%  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
%  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
%  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
%  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
%  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
%  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
%
%

%% New annotations
annotation assume(array[int] of var bool: b);
annotation int_priority(array[int] of var int: xs,
  array[int] of ann: br, ann: sel);
annotation bool_priority(array[int] of var bool: xs,
  array[int] of ann: br, ann: sel);

%predicate cumulative(array[int] of var int: s,
%  array[int] of var int: d,
%  array[int] of var int: r,
%  var int: b) =
%    geas_cumulative_var(s, d, r, b);
%
%predicate cumulative(array[int] of var int: s,
%  array[int] of int: d,
%  array[int] of int: r,
%  int: b) =
%    geas_cumulative(s, d, r, b);
%%  if b = 1 /\ forall (i in index_set(r)) (r[i] = 1) then
%%    geas_disjunctive(s, d)
%%  else
%%    geas_cumulative(s, d, r, b)
%%  endif;
%
%predicate geas_cumulative_var(array[int] of var int: s,
%  array[int] of var int: d,
%  array[int] of var int: r,
%  var int: b);
%predicate geas_cumulative(array[int] of var int: s,
%  array[int] of int: d,
%  array[int] of int: r,
%  int: b);

%% TODO: Fix disjunctive propagator
%predicate disjunctive(array[int] of var int: s, array[int] of int: d) =
%  % geas_disjunctive(s, d);
%  geas_cumulative(s, d, [1 | i in index_set(s)], 1);
%predicate geas_disjunctive(array[int] of var int: s, array[int] of int: d);
% Sums over Boolean variables
%predicate bool_lin_eq(array[int] of int: a, array[int] of var bool: x,
%                      var int: c);
%predicate bool_lin_ne(array[int] of int: a, array[int] of var bool: x,
%                      var int: c);
%predicate bool_lin_le(array[int] of int: a, array[int] of var bool: x,
%                      var int: c);
%predicate bool_lin_lt(array[int] of int: a, array[int] of var bool: x,
%                      var int: c);
%predicate bool_lin_ge(array[int] of int: a, array[int] of var bool: x,
%                      var int: c);
%predicate bool_lin_gt(array[int] of int: a, array[int] of var bool: x,
%                      var int: c);

%predicate bool_sum_eq(array[int] of var bool: x, var int: c); %=
%    bool_lin_eq([1 | i in index_set(x)],x,c);
%predicate bool_sum_ne(array[int] of var bool: x, var int: c); %=
%    bool_lin_ne([1 | i in index_set(x)],x,c);
%predicate bool_sum_le(array[int] of var bool: x, var int: c); %=
%    bool_lin_le([1 | i in index_set(x)],x,c);
%predicate bool_sum_lt(array[int] of var bool: x, var int: c); %=
%    bool_lin_lt([1 | i in index_set(x)],x,c);
%predicate bool_sum_ge(array[int] of var bool: x, var int: c); %=
%    bool_lin_ge([1 | i in index_set(x)],x,c);
%predicate bool_sum_gt(array[int] of var bool: x, var int: c); %=
%    bool_lin_gt([1 | i in index_set(x)],x,c);

%predicate array_int_maximum(var int: m, array[int] of var int: x);

%predicate int_eq_reif(var int: x, var int: y, var bool: r) =
%  let { var bool: a; var bool: b } in
%  (r <-> (a /\ b)) /\ int_le_reif(x, y, a) /\ int_le_reif(y, x, b);
%  %bool_clause([a],[r]) /\ bool_clause([b],[r]) /\ bool_clause([r], [a, b])
%  %/\ int_le_reif(x, y, a) /\ int_le_reif(y, x, b);

%predicate int_ne_reif(var int: x, var int: y, var bool: r) =
%  let { var bool: a; var bool: b } in
%  %bool_clause([], [r, a, b]) /\ bool_clause([a, r], []) /\ bool_clause([b, r], [])
%  (r <-> ((not a) \/ (not b)))
%  /\ int_le_reif(x, y, a) /\ int_le_reif(y, x, b);

predicate int_mul(var int: x, var int: y, var int: z);

%% Special cases for binary-ish x, y.

predicate int_lin_eq_reif(
  array [int] of int: cs, array [int] of var int: xs, int: k, var bool: r) =
  let { var bool: a; var bool: b } in
  (r <-> (a /\ b)) /\ int_lin_le_reif(cs, xs, k, a)
  /\ int_lin_le_reif([-c | c in cs], xs, -k, b);

%predicate int_lin_ne_reif(
%  array [int] of int: cs, array [int] of var int: xs, int: k, var bool: r) =
%  let { var bool: a; var bool: b } in
%  (r <-> ((not a) \/ (not b)))
%  /\ int_lin_le_reif(cs, xs, k, a)
%  /\ int_lin_le_reif([-c | c in cs], xs, -k, b);

%predicate bool_eq_reif(var bool: x, var bool: y, var bool: r) =
%  r <-> (x <-> y);
predicate bool_xor(var bool: x, var bool: y, var bool: r) =
     bool_clause([x,y],[r])
  /\ bool_clause([],[x,y,r])
  /\ bool_clause([x, r], [y])
  /\ bool_clause([y, r], [x]);

predicate set_in_reif(var int: x, set of int: s, var bool: r) =
  if card(s) = max(s) - min(s) + 1 then
    r <-> (x >= min(s) /\ x <= max(s)) 
  else
    bool_clause([x = k | k in s], [r]) /\ forall (k in s) (x = k -> r)
  endif;

predicate int_pow(var int: x, var int: y, var int: z) =
  let { array [dom(x), dom(y)] of int: A = array2d(dom(x), dom(y), [ pow(a, b) | a in dom(x), b in dom(y) ]) ; } in
  z = A[x, y];

predicate int_pow(var int: x, int: y, var int: z) =
  if y = 0 then z = 1
  elseif y = 1 then z = x
  else
    let { var int: zp ::is_defined_var ;
          constraint int_pow(x, y div 2, zp) :: defines_var(zp); } in
    if y mod 2 = 0 then
      z = zp * zp
    else
      z = x * zp * zp
    endif
  endif;

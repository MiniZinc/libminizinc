%
%  Main authors:
%     Guido Tack <tack@gecode.org>
%
%  Copyright:
%     Guido Tack, 2007
%
%  Last modified:
%     $Date: 2015-01-05 17:33:06 +1100 (Mon, 05 Jan 2015) $ by $Author: tack $
%     $Revision: 14337 $
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


/***
 @groupdef gecode Additional declarations for Gecode 
 
 These annotations and predicates are available for the Gecode
 solver. In order to use them in a model, include the file "gecode.mzn".
*/

/***
 @groupdef gecode.annotations Additional Gecode search annotations
*/

/** @group gecode.annotations Select variable with smallest accumulated failure count */
annotation afc_min;
/** @group gecode.annotations Select variable with smallest accumulated failure count divided by domain size */
annotation afc_size_min;
/** @group gecode.annotations Select variable with largest accumulated failure count */
annotation afc_max;
/** @group gecode.annotations Select variable with largest accumulated failure count divided by domain size */
annotation afc_size_max;
/** @group gecode.annotations Select variable with smallest activity count */
annotation activity_min;
/** @group gecode.annotations Select variable with smallest activity count divided by domain size */
annotation activity_size_min;
/** @group gecode.annotations Select variable with largest activity count */
annotation activity_max;
/** @group gecode.annotations Select variable with largest activity count divided by domain size */
annotation activity_size_max;
/** @group gecode.annotations Select random variable */
annotation random;

/** @group gecode.annotations Specify default search strategy for integer variables to use variable selection
  strategy \a varsel, and value choice strategy \a valsel. */
annotation int_default_search(ann: varsel, ann: valsel);
/** @group gecode.annotations Specify default search strategy for Boolean variables to use variable selection
  strategy \a varsel, and value choice strategy \a valsel. */
annotation bool_default_search(ann: varsel, ann: valsel);
/** @group gecode.annotations Specify default search strategy for set variables to use variable selection
  strategy \a varsel, and value choice strategy \a valsel. */
annotation set_default_search(ann: varsel, ann: valsel);
/** @group gecode.annotations Specify default search strategy for float variables to use variable selection
  strategy \a varsel, and value choice strategy \a valsel. */
annotation float_default_search(ann: varsel, ann: valsel);

/** @group gecode.annotations
  Simple large neighbourhood search strategy: upon restart, for each variable in \a x,
  the probability of it being fixed to the previous solution is \a percentage (out of 100).
*/
annotation relax_and_reconstruct(array[int] of var int: x, int: percentage);

/** @group gecode.annotations
  Simple large neighbourhood search strategy: upon restart, for each variable in \a x,
  the probability of it being fixed to the previous solution is \a percentage (out of 100).
  Start from an initial solution \a y.
*/
annotation relax_and_reconstruct(array[int] of var int: x, int: percentage, array[int] of int: y);

/***
 @groupdef gecode.constraints Additional Gecode constraints
*/

/** @group gecode.constraints
  Constrain \a z to be the intersection of all sets
  in \a y that are selected by \a x: \(i \in \a z \leftrightarrow \forall j \in \a x: (i \in \a y[j]) \)
*/
predicate gecode_array_set_element_intersect(var set of int: x,
    array[int] of var set of int: y, var set of int: z);

/** @group gecode.constraints
  Constrain \a z to be the disjoint union of all sets
  in \a y that are selected by \a x: \(i \in \a z \leftrightarrow \exists j \in \a x: (i \in \a y[j]) \)
  and \( i \in \a x \land j \in \a x \land i\neq j \rightarrow \a y[i] \cap \a y[j]=\emptyset \)
*/
predicate gecode_array_set_element_partition(var set of int: x,
    array[int] of var set of int: y, var set of int: z);

/** @group gecode.constraints
  Constrain \a z to be a subset of \a u, and \a z to be the intersection of all sets
  in \a y that are selected by \a x: \(i \in \a z \leftrightarrow \forall j \in \a x: (i \in \a y[j]) \)
*/
predicate gecode_array_set_element_intersect_in(var set of int: x,
    array[int] of var set of int: y,
    var set of int: z, set of int: u);

predicate gecode_among_seq_int(array[int] of var int: x, set of int: S,
    int: l, int: m, int: n);

predicate gecode_among_seq_bool(array[int] of var bool: x, bool: b,
    int: l, int: m, int: n);

/** @group gecode.constraints
 Every subsequence of \a x of length \a l has at least \a m and at most \a n occurrences
 of the values in \a S
*/
predicate among_seq(array[int] of var int: x, set of int: S,
    int: l, int: m, int: n) = gecode_among_seq_int(x,S,l,m,n);
/** @group gecode.constraints
 Every subsequence of \a x of length \a l has at least \a m and at most \a n occurrences
 of the values in \a S
*/
predicate among_seq(array[int] of var bool: x, bool: b,
    int: l, int: m, int: n) = gecode_among_seq_bool(x,b,l,m,n);

predicate gecode_circuit_cost_array(array[int] of int: c,
    array[int] of var int: x, array[int] of var int: y, var int: z);
predicate gecode_circuit_cost(array[int] of int: c, array[int] of var int: x,
    var int: z);

/** @group gecode.constraints
  Constrains the elements of \a x to define a circuit where \a x[\p i] = \p j means
  that \p j is the successor of \p i. Additionally, constrain \a z to
  be the cost of the circuit. Each edge cost is defined by array \a c. The variables
  \a y[i] are constrained to be the edge cost of the node \a x[i].
*/
predicate circuit_cost_array(array[int] of int: c,
    array[int] of var int: x, array[int] of var int: y, var int: z) =
  gecode_circuit_cost_array(c,[x[i]-min(index_set(x)) | i in index_set(x)],
                            y,z);
/** @group gecode.constraints
  Constrains the elements of \a x to define a circuit where \a x[\p i] = \p j means
  that \p j is the successor of \p i. Additionally, constrain \a z to
  be the cost of the circuit. Each edge cost is defined by array \a c.
*/
predicate circuit_cost(array[int] of int: c, array[int] of var int: x,
    var int: z) =
  gecode_circuit_cost(c, [x[i]-min(index_set(x)) | i in index_set(x)], z);

predicate gecode_schedule_unary(array[int] of var int: x, array[int] of int: p);
predicate gecode_schedule_unary_optional(array[int] of var int: x,
    array[int] of int: p, array[int] of var bool: m);

predicate gecode_schedule_cumulative_optional(array[int] of var int: start,
  array[int] of int: duration, array[int] of int: usage,
  array[int] of var bool: m, int: capacity);

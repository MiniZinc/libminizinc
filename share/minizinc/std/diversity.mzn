/***
 @groupdef diversity MiniZinc definitions for the MiniZinc diversity extension.

 These annotations and predicates can be used to produce diverse set of
 solutions.In order to use them in a model, include the file "diversity.mzn".
*/

/***
 @groupdef diversity.annotations Annotations provided to guide the MiniZinc diversity extension.
*/

/** @group diversity.annotations Produce at most \a k number of diverse solutions with thier objective values 
  bounded between optimal value and \a gap % from the optimal. Diverse solutions are obtained one after the other
  and distance to k-th diverse solution is sum of pairwise distance to all the previous diverse solutions 
*/
annotation diversity_incremental(int: k, float: gap);
/** @group diversity.annotations
  Use \a distance_metric on \a x when computing distance to solutions. Here same index on different solutions are compared.
  For example, when obtaining the distance to k-th diverse solution using the incremental approach (diversity_incremental) each index of 
  \a x in k-th solution is compared to the corresponding index of all previously obtained diverse solutions. 
*/
annotation diverse_pairwise(array [int] of var int: x, string: distance_metric);
/** @group diversity.annotations
  Use \a distance_metric on \a x when computing distance to solutions. Here same index on different solutions are compared.
  For example, when obtaining the distance to k-th diverse solution using the incremental approach (diversity_incremental) each index of 
  \a x in k-th solution is compared to the corresponding index of all previously obtained diverse solutions. 
*/
function ann: diverse_pairwise(array [$T] of var int: x, string: distance_metric) =
  diverse_pairwise(array1d(x), distance_metric);
/** @group diversity.annotations
  Use \a distance_metric on \a x to compute the distance to solutions, and apply constraints using \a min_dist to enforce a minimum
  distance and \a max_dist to enforce a maximum distance. The \a weight parameter can be used to set relative priorities between
  multiple distance measures. Distance measures with higher weights are given greater priority.
  For example, when obtaining the distance to k-th diverse solution using the incremental approach (diversity_incremental) each index of
  \a x in k-th solution is compared to the corresponding index of all previously obtained diverse solutions.
*/
annotation diverse_pairwise(
  array [int] of var int: x,
  string: distance_metric,
  int: min_dist,
  int: max_dist,
  int: weight,
);
/** @group diversity.annotations
  Use \a distance_metric on \a x to compute the distance to solutions, and apply constraints using \a min_dist to enforce a minimum
  distance and \a max_dist to enforce a maximum distance. The \a weight parameter can be used to set relative priorities between
  multiple distance measures. Distance measures with higher weights are given greater priority.
  For example, when obtaining the distance to k-th diverse solution using the incremental approach (diversity_incremental) each index of
  \a x in k-th solution is compared to the corresponding index of all previously obtained diverse solutions.
*/
function ann: diverse_pairwise(
  array [$T] of var int: x,
  string: distance_metric,
  int: min_dist,
  int: max_dist,
  int: weight,
) = diverse_pairwise(array1d(x), distance_metric, min_dist, max_dist, weight);
/** @group diversity.annotations
  Use \a distance_metric on \a x when computing distance to solutions. Here same index on different solutions are compared.
  For example, when obtaining the distance to k-th diverse solution using the incremental approach (diversity_incremental) each index of 
  \a x in k-th solution is compared to the corresponding index of all previously obtained diverse solutions.
*/
annotation diverse_pairwise(array [int] of var float: x, string: distance_metric);
/** @group diversity.annotations
  Use \a distance_metric on \a x when computing distance to solutions. Here same index on different solutions are compared.
  For example, when obtaining the distance to k-th diverse solution using the incremental approach (diversity_incremental) each index of 
  \a x in k-th solution is compared to the corresponding index of all previously obtained diverse solutions. 
*/
function ann: diverse_pairwise(array [$T] of var float: x, string: distance_metric) =
  diverse_pairwise(array1d(x), distance_metric);
/** @group diversity.annotations
  Use \a distance_metric on \a x to compute the distance to solutions, and apply constraints using \a min_dist to enforce a minimum
  distance and \a max_dist to enforce a maximum distance. The \a weight parameter can be used to set relative priorities between
  multiple distance measures. Distance measures with higher weights are given greater priority.
  For example, when obtaining the distance to k-th diverse solution using the incremental approach (diversity_incremental) each index of
  \a x in k-th solution is compared to the corresponding index of all previously obtained diverse solutions.
*/
annotation diverse_pairwise(
  array [int] of var float: x,
  string: distance_metric,
  float: min_dist,
  float: max_dist,
  float: weight,
);
/** @group diversity.annotations
  Use \a distance_metric on \a x to compute the distance to solutions, and apply constraints using \a min_dist to enforce a minimum
  distance and \a max_dist to enforce a maximum distance. The \a weight parameter can be used to set relative priorities between
  multiple distance measures. Distance measures with higher weights are given greater priority.
  For example, when obtaining the distance to k-th diverse solution using the incremental approach (diversity_incremental) each index of
  \a x in k-th solution is compared to the corresponding index of all previously obtained diverse solutions.
*/
function ann: diverse_pairwise(
  array [$T] of var float: x,
  string: distance_metric,
  float: min_dist,
  float: max_dist,
  float: weight,
) = diverse_pairwise(array1d(x), distance_metric, min_dist, max_dist, weight);
/** @group diversity.annotations
  An aggregator combines pairwise diversity measures from each pair of solutions into an overall diversity score to identify the k most
  diverse solutions, using an incremental approach (i.e., diversity_incremental). Currently, it supports three methods: sum, which sums
  up the pairwise diversity measures; min, which selects the minimum; and max, which selects the maximum.
*/
annotation diversity_aggregator(string);
/** @group diversity.annotations
  Use to combine distance functions to diversity of a diverse solution. Currently, it supports three methods: sum, which sums   up the
  distance measures; min, which selects the minimum; and max, which selects the maximum.
*/
annotation diversity_combinator(string);

/***
 @groupdef diversity.distance Distance functions provided in the MiniZinc diversity extension.
*/

/** @group diversity.distance
  Returns the Hamming distance between \a x and \a y.
*/
function int: hamming_distance(array [$A] of opt $T: x, array [$A] of opt $T: y) =
  assert(
    index_sets_agree(x, y),
    "hamming_distance: x and y must have identical index sets",
    let {
      any: xx = array1d(x);
      any: yy = array1d(y);
    } in count (i in index_set(array1d(xx))) (xx[i] != yy[i]),
  );

/** @group diversity.distance
  Returns the Hamming distance between \a x and \a y.
*/
function var int: hamming_distance(array [$A] of var opt $T: x, array [$A] of var opt $T: y) =
  assert(
    index_sets_agree(x, y),
    "hamming_distance: x and y must have identical index sets",
    let {
      any: xx = array1d(x);
      any: yy = array1d(y);
    } in count (i in index_set(array1d(xx))) (xx[i] != yy[i]),
  );

/** @group diversity.distance
  Returns the Hamming distance between \a x and \a y.
*/
function int: manhattan_distance(array [$A] of $$T: x, array [$A] of $$T: y) =
  assert(
    index_sets_agree(x, y),
    "manhattan_distance: x and y must have identical index sets",
    let {
      any: xx = array1d(x);
      any: yy = array1d(y);
    } in sum (i in index_set(xx)) (abs(xx[i] - yy[i])),
  );

/** @group diversity.distance
  Returns the Hamming distance between \a x and \a y.
*/
function var int: manhattan_distance(array [$A] of var $$T: x, array [$A] of var $$T: y) =
  assert(
    index_sets_agree(x, y),
    "manhattan_distance: x and y must have identical index sets",
    let {
      any: xx = array1d(x);
      any: yy = array1d(y);
    } in sum (i in index_set(xx)) (abs(xx[i] - yy[i])),
  );

/** @group diversity.distance
  Returns the Hamming distance between \a x and \a y.
*/
function float: manhattan_distance(array [$A] of float: x, array [$A] of float: y) =
  assert(
    index_sets_agree(x, y),
    "manhattan_distance: x and y must have identical index sets",
    let {
      any: xx = array1d(x);
      any: yy = array1d(y);
    } in sum (i in index_set(xx)) (abs(xx[i] - yy[i])),
  );

/** @group diversity.distance
  Returns the Hamming distance between \a x and \a y.
*/
function var float: manhattan_distance(array [$A] of var float: x, array [$A] of var float: y) =
  assert(
    index_sets_agree(x, y),
    "manhattan_distance: x and y must have identical index sets",
    let {
      any: xx = array1d(x);
      any: yy = array1d(y);
    } in sum (i in index_set(xx)) (abs(xx[i] - yy[i])),
  );

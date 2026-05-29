include "fzn_knapsack.mzn";
include "fzn_knapsack_reif.mzn";

/**  @group globals.packing
  Requires that items are packed in a knapsack with certain weight and profit restrictions.

  Assumptions:

  - Weights \a weights and profits \a profits must be non-negative
  - \a weights, \a profits and \a xs must have the same index sets

@param weights: weight of each type of item
@param profits: profit of each type of item
@param xs: number of items of each type that are packed
@param total_weight: sum of sizes of all items in the knapsack
@param total_profit: sum of profits of all items in the knapsack
*/
predicate knapsack(
  array [$$I] of int: weights,
  array [$$I] of int: profits,
  array [$$I] of var int: xs,
  var int: total_weight,
  var int: total_profit,
) =
  assert(
    index_set(weights) = index_set(profits) /\ index_set(weights) = index_set(xs),
    "index set of weights must be equal to index set of profits and index set of items",
    assert(
      lb_array(weights) >= 0,
      "weights must be non-negative",
      assert(
        lb_array(profits) >= 0,
        "profits must be non-negative",
        fzn_knapsack(index2int(weights), index2int(profits), index2int(xs), total_weight, total_profit),
      ),
    ),
  );

include "fzn_knapsack.mzn";
include "fzn_knapsack_reif.mzn";

predicate fzn_knapsack(
  array [int] of int: weight,
  array [int] of int: profit,
  array [int] of var int: xs,
  var int: total_weight,
  var int: total_profit,
);

/**  @group globals.packing
  Requires that items are packed in a knapsack with certain weight and profit restrictions.

  Assumptions:

  - Weights \a weight and profit \a profit must be non-negative
  - \a weight, \a profit and \a xs must have the same index sets

@param weight: weight of each type of item
@param profit: profit of each type of item
@param xs: number of items of each type that are packed
@param total_weight: sum of sizes of all items in the knapsack
@param total_profit: sum of profit of all items in the knapsack
*/
predicate knapsack(
  array [$$I] of int: weight,
  array [$$I] of int: profit,
  array [$$I] of var int: xs,
  var int: total_weight,
  var int: total_profit,
) =
  assert(
    index_set(weight) = index_set(profit) /\ index_set(weight) = index_set(xs),
    "index set of weight must be equal to index set of profit and index set of items",
    assert(
      lb_array(weight) >= 0,
      "weight must be non-negative",
      assert(
        lb_array(profit) >= 0,
        "profit must be non-negative",
        fzn_knapsack(index2int(weight), index2int(profit), index2int(xs), total_weight, total_profit),
      ),
    ),
  );

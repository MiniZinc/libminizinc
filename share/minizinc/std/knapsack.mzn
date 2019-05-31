include "fzn_knapsack.mzn";

/**  @group globals.packing
Requires that items are packed in a knapsack with certain weight and profit restrictions.

Assumptions:
  - Weights \a w and profits \a p must be non-negative
  - \a w, \a p and \a x must have the same index sets

@param w: weight of each type of item
@param p: profit of each type of item
@param x: number of items of each type that are packed
@param W: sum of sizes of all items in the knapsack
@param P: sum of profits of all items in the knapsack
*/
predicate knapsack(array[int] of int: w, array[int] of int:p, 
                   array[int] of var int:x, var int: W, var int: P) = 
	assert(index_set(w) = index_set(p) /\ index_set(w) = index_set(x),
		"index set of weights must be equal to index set of profits and index set of items",
	assert(lb_array(w) >= 0,
		"weights must be non-negative",
	assert(lb_array(p) >= 0,
		"profits must be non-negative",
    fzn_knapsack(w, p, x, W, P)
	)));

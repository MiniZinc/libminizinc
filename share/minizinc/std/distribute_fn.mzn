include "distribute.mzn";

/** @group globals.counting
  Returns an array of the number of occurrences of \a value[\p i] in
  \a base. The values in \a value need not be distinct.
*/
function array[int] of var int: distribute(array[int] of var int: value,
                                           array[int] of var int: base) :: promise_total =
  let { array[index_set(value)] of var 0..length(base): card;
        constraint distribute(card, value, base); } 
  in card;

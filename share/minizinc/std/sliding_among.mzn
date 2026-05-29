include "fzn_sliding_among_int.mzn";
include "fzn_sliding_among_bool.mzn";

/**  @group globals.counting
 Every subsequence of \a xs of length \a window_size has at least \a low and at most \a up occurrences
 of the values in \a values
*/
predicate sliding_among(int: low, int: up, int: window_size, array [$X] of var $$E: xs, set of $$E: values) =
  fzn_sliding_among_int(low, up, window_size, enum2int(array1d(xs)), enum2int(values));

/**  @group globals.counting
 Every subsequence of \a xs of length \a window_size has at least \a low and at most \a up occurrences
 of the value \a value
*/
predicate sliding_among(int: low, int: up, int: window_size, array [$X] of var bool: xs, bool: value) =
  fzn_sliding_among_bool(low, up, window_size, array1d(xs), value);

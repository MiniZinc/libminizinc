include "fzn_cumulatives_decomp.mzn";

% min_m is the first machine (which is the minimum, of the index set of b).
% For native implementations, you can subtract (min_m - 1) from the m array
% values to obtain 1-based machine numbers.
predicate fzn_cumulatives(array[int] of var int: s,
                          array[int] of var int: d,
                          array[int] of var int: r,
                          array[int] of var int: m,
                          array[int] of var int: b,
                          bool: upper,
                          int: min_m) =
  fzn_cumulatives_decomp(s, d, r, m, b, upper);

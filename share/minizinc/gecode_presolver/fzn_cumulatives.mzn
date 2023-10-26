include "fzn_cumulatives_decomp.mzn";

predicate fzn_cumulatives(array[int] of var int: s,
                          array[int] of var int: d,
                          array[int] of var int: r,
                          array[int] of var int: m,
                          array[int] of var int: b,
                          bool: upper,
                          int: min_m) =
  if is_fixed(b) then
    % Propagator only supports fixed bounds
    gecode_cumulatives(s, d, r, [m_i - min_m | m_i in m], fix(b), upper)
  else
    fzn_cumulatives_decomp(s, d, r, m, b, upper)
  endif;

predicate gecode_cumulatives(array[int] of var int: s,
                      array[int] of var int: d,
                      array[int] of var int: r,
                      array[int] of var int: m,
                      array[int] of int: b,
                      bool: upper);

include "fzn_cumulatives_decomp.mzn";

predicate fzn_cumulatives(
  array [int] of var int: starts,
  array [int] of var int: durations,
  array [int] of var int: resources,
  array [int] of var int: machines,
  array [int] of var int: bounds,
  bool: is_upper_bound,
  int: min_machine,
) =
  if is_fixed(bounds) then
    % Propagator only supports fixed bounds
    gecode_cumulatives(
      starts,
      durations,
      resources,
      [m_i - min_machine | m_i in machines],
      fix(bounds),
      is_upper_bound,
    )
  else
    fzn_cumulatives_decomp(starts, durations, resources, machines, bounds, is_upper_bound)
  endif;

predicate gecode_cumulatives(
  array [int] of var int: starts,
  array [int] of var int: durations,
  array [int] of var int: resources,
  array [int] of var int: machines,
  array [int] of int: bounds,
  bool: is_upper_bound,
);

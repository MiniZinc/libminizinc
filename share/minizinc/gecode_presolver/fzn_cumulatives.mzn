include "fzn_cumulatives_decomp.mzn";

predicate fzn_cumulatives(
  array [int] of var int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  array [int] of var int: machine,
  array [int] of var int: bound,
  bool: is_upper_bound,
  int: min_machine,
) =
  if is_fixed(bound) then
    % Propagator only supports fixed bound
    gecode_cumulatives(
      start,
      duration,
      resource,
      [m_i - min_machine | m_i in machine],
      fix(bound),
      is_upper_bound,
    )
  else
    fzn_cumulatives_decomp(start, duration, resource, machine, bound, is_upper_bound)
  endif;

predicate gecode_cumulatives(
  array [int] of var int: start,
  array [int] of var int: duration,
  array [int] of var int: resource,
  array [int] of var int: machine,
  array [int] of int: bound,
  bool: is_upper_bound,
);

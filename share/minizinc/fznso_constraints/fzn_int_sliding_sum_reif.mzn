% The constraint `int_sliding_sum_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_sliding_sum_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_int_sliding_sum_reif(
  list of var int: xs,
  int: window,
  int: lower_bound,
  int: upper_bound,
  var bool: b,
) =
  /* CS decomposition: see S. Brand, N. Narodytska, C-G. Quimper, P.J. Stuckey, and T. Walsh. Encodings of the sequence constraint. In C. Bessiere, editor, Proceedings of the 13th International Conference on Principles and Practice of Constraint Programming, volume 4741 of LNCS, pages 210–224. Springer-Verlag, 2007.
    */
  let {
    int: lx = min(index_set(xs));
    int: ux = max(index_set(xs));
    array [lx - 1..ux] of var int: S;
  } in S[lx - 1] = 0 /\ forall (i in lx..ux) (S[i] = xs[i] + S[i - 1]) /\ b <->
    forall (i in lx - 1..ux - window) (S[i] <= S[i + window] - lower_bound /\ S[i + window] <= S[i] + upper_bound);

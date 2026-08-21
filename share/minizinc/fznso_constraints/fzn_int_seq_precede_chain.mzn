% The constraint `int_seq_precede_chain`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_seq_precede_chain` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

predicate fzn_int_seq_precede_chain(list of var int: xs) =
  if length(xs) = 0 then
    true
  else
    let {
      int: l = lb_array(xs); % least possible value
      int: u = ub_array(xs); % greatest possible value
      int: f = min(index_set(xs));
      array [index_set(xs)] of var l..u: H;
    } in H[f] <= 1 /\
      H[f] = max(xs[f], 0) /\
      forall (i in index_set(xs) diff {f}) (H[i] <= H[i - 1] + 1 /\ H[i] = max(xs[i], H[i - 1]))
  endif;

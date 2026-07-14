%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is strictly lexicographically less than array 'ys'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_less_int(
  array [int] of var int: xs :: promise_ctx_antitone,
  array [int] of var int: ys :: promise_ctx_monotone,
) = lex_less_std_decomposition(xs, ys);

%-----------------------------------------------------------------------------%

include "count_fn.mzn";

predicate fzn_count_geq(array[int] of var int: x, var int: y, var int: c) =
  % This needs to be written with a let rather than count(x,y) <= c
  % so that the automatic rewriting of the latter doesn't kick in
  % TODO: This is probably the better way to do this, but still always gives
  % reifications (instead of half-reifications)
  % let {
  %   var int: z ::promise_ctx_antitone = count(xi in x)(xi == y);
  % } in z <= c;
  let {
    var int: z ::promise_ctx_monotone = count(xi in x)(xi != y);
  } in z >= (length(x) - c);

%-----------------------------------------------------------------------------%

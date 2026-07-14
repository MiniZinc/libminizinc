predicate fzn_range_reif(
  array [int] of var int: xs,
  var set of int: domain,
  var set of int: image,
  var bool: b,
) =
  b <->
    (
      % All values in 's' must map to a value in 't'.
      forall (i in ub(domain)) (i in domain -> xs[i] in image) /\
        % All values in 't' must be mapped from a value in 's'.
        forall (i in ub(image)) (i in image -> exists (j in ub(domain)) (j in domain /\ xs[j] == i))
    );

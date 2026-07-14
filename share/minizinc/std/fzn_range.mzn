predicate fzn_range(array [int] of var int: xs, var set of int: domain, var set of int: image) =
  % All values in 'domain' must map to a value in 'image'.
  forall (i in ub(domain)) (i in domain -> xs[i] in image) /\
    % All values in 'image' must be mapped from a value in 'domain'.
    forall (i in ub(image)) (
      i in image -> exists (j in ub(domain)) (j in domain /\ xs[j] == i)
    );

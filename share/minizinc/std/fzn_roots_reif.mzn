predicate fzn_roots_reif(
  array [int] of var int: xs,
  var set of int: preimage,
  var set of int: image,
  var bool: b,
) =
  b <->
    (
      % All values in 'preimage' must map to a value in 'image'.
      forall (i in ub(preimage)) (i in preimage -> xs[i] in image) /\
        forall (i in ub(image)) (
          i in image -> forall (j in index_set(xs)) (xs[j] = i -> j in preimage)
        )
    );

predicate fzn_cluster_reif(array[$$E] of $$N: from, array[$$E] of $$N: to,
                           array[$$N,$$N] of var bool: r, array[$$N] of var bool: ns,
                           array[$$E] of var bool: es, var bool: b) =
  abort("Reified cluster constraint is not supported");

%-----------------------------------------------------------------------------%

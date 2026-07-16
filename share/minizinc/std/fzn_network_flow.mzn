predicate fzn_network_flow(
  array [int, 1..2] of int: arcs,
  array [int] of int: balance,
  array [int] of var int: flow,
) =
  let {
    int: source_node = 1;
    int: sink_node = 2;
    set of int: ARCS = index_set_1of2(arcs);
    set of int: NODES = index_set(balance);
  } in forall (i in NODES) (
    sum (j in ARCS where i == arcs[j, source_node]) (flow[j]) -
      sum (j in ARCS where i == arcs[j, sink_node]) (flow[j]) =
      balance[i]
  );

predicate fzn_network_flow_cost(
  array [int, 1..2] of int: arcs,
  array [int] of int: balances,
  array [int] of int: weights,
  array [int] of var int: flows,
  var int: total_cost,
) =
  let {
    int: source_node = 1;
    int: sink_node = 2;
    set of int: ARCS = index_set_1of2(arcs);
    set of int: NODES = index_set(balances);
  } in total_cost = sum (i in ARCS) (flows[i] * weights[i]) /\
    forall (i in NODES) (
      sum (j in ARCS where i == arcs[j, source_node]) (flows[j]) -
        sum (j in ARCS where i == arcs[j, sink_node]) (flows[j]) =
        balances[i]
    );

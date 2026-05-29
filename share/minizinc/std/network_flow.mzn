include "fzn_network_flow.mzn";
include "fzn_network_flow_reif.mzn";
include "fzn_network_flow_cost.mzn";
include "fzn_network_flow_cost_reif.mzn";

/** @group globals.graph
  Defines a network flow constraint.

@param arcs: the directed arcs of the flow network. Arc \p i connects node \a arcs[\p i,1] to node \a arcs[\p i,2].
@param balances: the amount of flow added or removed by each node. If the balance is positive then flow is added by the node, and it is removed when it is negative, i.e., ``balance = output - input``.
@param flows: the flow going through each arc.
*/
predicate network_flow(
  array [$$E, 1..2] of $$N: arcs,
  array [$$N] of int: balances,
  array [$$E] of var int: flows,
) =
  let {
    any: ARCS = index_set_1of2(arcs);
    any: NODES = index_set(balances);
  } in assert(
    ARCS == index_set(flows),
    "network_flow: index set of arcs, \(ARCS), does not match the index set of flow, \(
      index_set(flows)
    )",
  ) /\
    assert(
      dom_bounds_array(arcs) subset NODES,
      "network_flow: some nodes in arcs, \(
        dom_bounds_array(arcs)
      ), do not match the index set of balance \(NODES)",
    ) /\
    if sum(balances) != 0 then
      false
    else
      fzn_network_flow(index2int(enum2int(arcs)), index2int(balances), index2int(flows))
    endif;

/** @group globals.graph
  Defines a network flow constraint with cost.

@param arcs: the directed arcs of the flow network. Arc \p i connects node \a arcs[\p i,1] to node \a arcs[\p i,2].
@param balances: the amount of flow added or removed by each node. If the balance is positive then flow is added by the node, and it is removed when it is negative, i.e., ``balance = output - input``.
@param weights: the unit cost of the flow through the arc.
@param flows: the flow going through each arc.
@param total_cost: the overall cost of the flow.
*/
predicate network_flow_cost(
  array [$$E, 1..2] of $$N: arcs,
  array [$$N] of int: balances,
  array [$$E] of int: weights,
  array [$$E] of var int: flows,
  var int: total_cost,
) =
  let {
    any: ARCS = index_set_1of2(arcs);
    any: NODES = index_set(balances);
  } in assert(
    ARCS == index_set(flows),
    "network_flow_cost: index set of arcs, \(ARCS), does not match the index set of flow, \(
      index_set(flows)
    )",
  ) /\
    assert(
      ARCS == index_set(weights),
      "network_flow_cost: index set of arcs, \(ARCS), does not match the index set of weight, \(
        index_set(weights)
      )",
    ) /\
    assert(
      dom_bounds_array(arcs) subset NODES,
      "network_flow_cost: some nodes in arcs, \(
        dom_bounds_array(arcs)
      ), do not match the index set of balance \(NODES)",
    ) /\
    if sum(balances) != 0 then
      false
    else
      fzn_network_flow_cost(
        index2int(enum2int(arcs)),
        index2int(balances),
        index2int(weights),
        index2int(flows),
        total_cost,
      )
    endif;

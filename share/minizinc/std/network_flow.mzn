include "fzn_network_flow.mzn";
include "fzn_network_flow_reif.mzn";
include "fzn_network_flow_cost.mzn";
include "fzn_network_flow_cost_reif.mzn";

/** @group globals.graph
  Defines a network flow constraint.
  
@param arc: a directed arc of the flow network. Arc \p i connects node \a arc[\p i,1] to node \a arc[\p i,2].
@param balance: the amount of flow added or removed by each node. If the balance is positive then flow is added by the node, and it is removed when it is negative, i.e., ``balance = output - input``. 
@param flow: the flow going through each arc.
*/
predicate network_flow(
	array[$$E, 1..2] of $$N: arc, 
	array[$$N] of int: balance,
	array[$$E] of var int: flow
) = 
	let {
		set of int: ARCS = index_set_1of2(arc);
		set of int: NODES = index_set(balance);
	} in
	assert(ARCS == index_set(flow), "network_flow: index set of arcs, \(ARCS), does not match the index set of flow, \(index_set(flow))") /\
	assert(dom_bounds_array(arc) subset NODES, "network_flow: some nodes in arcs, \(dom_bounds_array(arc)), do not match the index set of balance \(NODES)") /\
	if sum(balance) != 0 then
		false
	else
		fzn_network_flow(arc, balance, flow)
	endif;

/** @group globals.graph
  Defines a network flow constraint with cost.
  
@param arc: a directed arc of the flow network. Arc \p i connects node \a arc[\p i,1] to node \a arc[\p i,2].
@param balance: the amount of flow added or removed by each node. If the balance is positive then flow is added by the node, and it is removed when it is negative, i.e., ``balance = output - input``. 
@param weight: the unit cost of the flow through the arc.
@param flow: the flow going through each arc.
@param cost: the overall cost of the flow.
*/
predicate network_flow_cost(
	array[$$E, 1..2] of $$N: arc, 
	array[$$N] of int: balance,
	array[$$E] of int: weight,
	array[$$E] of var int: flow,
	var int: cost
) = 
	let {
		set of int: ARCS = index_set_1of2(arc);
		set of int: NODES = index_set(balance);
	} in
	assert(ARCS == index_set(flow), "network_flow_cost: index set of arcs, \(ARCS), does not match the index set of flow, \(index_set(flow))") /\
	assert(ARCS == index_set(weight), "network_flow_cost: index set of arcs, \(ARCS), does not match the index set of weight, \(index_set(weight))") /\
	assert(dom_bounds_array(arc) subset NODES, "network_flow_cost: some nodes in arcs, \(dom_bounds_array(arc)), do not match the index set of balance \(NODES)") /\
	if sum(balance) != 0 then
		false
	else
		fzn_network_flow_cost(arc, balance, weight, flow, cost)
	endif;

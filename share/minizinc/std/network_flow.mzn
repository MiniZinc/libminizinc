include "fzn_network_flow.mzn";
include "fzn_network_flow_cost.mzn";

/** @group globals
  Defines a network flow constraint.
  
@param arc: a directed arc of the flow network. Arc \p i connects node \a arc[\p i,1] to node \a arc[\p i,2].
@param balance: the difference between input and output flow for each node.
@param flow: the flow going through each arc.
*/

predicate network_flow(array[int,1..2] of int: arc, 
	  	                 array[int] of int: balance,
                       array[int] of var int: flow) = 

    let { 
          set of int: ARCS = index_set_1of2(arc);
          set of int: NODES = index_set(balance);
    } in
  	assert (
	   ARCS == index_set(flow) /\
	   lb_array(arc) >= min(NODES) /\ 
	   ub_array(arc) <= max(NODES),
	   "network_flow: wrong sizes of input array parameters",
          fzn_network_flow(arc, balance, flow)
	);

/** @group globals
  Defines a network flow constraint with cost.
  
@param arc: a directed arc of the flow network. Arc \p i connects node \a arc[\p i,1] to node \a arc[\p i,2].
@param balance: the difference between input and output flow for each node.
@param weight: the unit cost of the flow through the arc.
@param flow: the flow going through each arc.
@param cost: the overall cost of the flow.
*/
predicate network_flow_cost(array[int,1..2] of int: arc, 
	  	      array[int] of int: balance,
	  	      array[int] of int: weight,
            array[int] of var int: flow, var int: cost) = 

    let { 
          set of int: ARCS = index_set_1of2(arc);
          set of int: NODES = index_set(balance);
    } in
  	assert (
	   ARCS == index_set(flow) /\
	   ARCS == index_set(weight) /\
	   lb_array(arc) >= min(NODES) /\ 
	   ub_array(arc) <= max(NODES),
	   "network_flow: wrong sizes of input array parameters",
	fzn_network_flow_cost(arc, balance, weight, flow, cost)
	);

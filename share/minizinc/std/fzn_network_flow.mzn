/** @group globals
  Defines a network flow constraint.
  
@param arc: a directed arc of the flow network. Arc \p i connects node \a arc[\p i,1] to node \a arc[\p i,2].
@param balance: the difference between input and output flow for each node.
@param flow: the flow going through each arc.
*/

predicate fzn_network_flow(array[int,1..2] of int: arc, 
	  	                 array[int] of int: balance,
                       array[int] of var int: flow) = 

    let { int: source_node = 1;
          int: sink_node = 2;
          set of int: ARCS = index_set_1of2(arc);
          set of int: NODES = index_set(balance);
    } in
	   forall (i in NODES) (
	      sum (j in ARCS where i == arc[j,source_node]) (flow[j]) - 
        sum (j in ARCS where i == arc[j,sink_node]) (flow[j])
        = balance[i] 
     );

/** @group globals
  Defines a network flow constraint with cost.
  
@param arc: a directed arc of the flow network. Arc \p i connects node \a arc[\p i,1] to node \a arc[\p i,2].
@param balance: the difference between input and output flow for each node.
@param weight: the unit cost of the flow through the arc.
@param flow: the flow going through each arc.
@param cost: the overall cost of the flow.
*/
predicate fzn_network_flow_cost(array[int,1..2] of int: arc, 
	  	      array[int] of int: balance,
	  	      array[int] of int: weight,
            array[int] of var int: flow, var int: cost) = 

    let { int: source_node = 1;
          int: sink_node = 2;
          set of int: ARCS = index_set_1of2(arc);
          set of int: NODES = index_set(balance);
    } in
	   cost = sum(i in ARCS) (flow[i] * weight[i]) 
	   /\
	   forall (i in NODES) (
	      sum (j in ARCS where i == arc[j,source_node]) (flow[j]) - 
        sum (j in ARCS where i == arc[j,sink_node]) (flow[j])
        = balance[i] 
	);

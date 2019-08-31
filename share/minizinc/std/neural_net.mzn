include "fzn_neural_net.mzn";
include "fzn_neural_net_reif.mzn";


enum NEURON_TYPE = { NT_RELU, NT_STEP, NT_LINEAR, NT_SOFTPLUS };

/** @group globals.learning
  constrain the output layer of a neural net to take the value defined by the input layer.
  the arguments are
  \a inputs: an array of float variables
  \a input_ids: array[int] of node
  \a outputs: an array of float variables
  \a output_ids: array[int] of node
  \a bias: array[node] of float
  \a edge_weight: array[edge] of float (dummy one at end!)
  \a edge_parent: array[edge] of neuron (start neuron for edge)
  \a first_edge: array[node] of 1..m+1
  \a neuron_type: { NT_RELU, NT_STEP, NT_LINEAR, NT_SOFTPLUS }
*/
predicate neural_net(array[int] of var float: inputs,
                     array[int] of int: input_ids,
                     array[int] of var float: outputs,
                     array[int] of int: output_ids,
                     array[int] of float: bias,
                     array[int] of float: edge_weight,
                     array[int] of int: edge_parent,
                     array[int] of int: first_edge,
                     NEURON_TYPE: neuron_type) =
    assert(index_set(inputs) == index_set(input_ids),
                 "neural_net: number of input vars not equal to number of input_ids") /\
    assert(index_set(outputs) == index_set(output_ids),
                 "neural_net: number of output vars not equal to number of output_ids") /\
          let { set of int: node = index_set(bias) } in
          let { set of int: edge = index_set(edge_weight) } in
          assert(index_set(edge_parent) == edge,
                 "neural_net: index sets of edge_weight and edge_parent do not agree") /\
    assert(index_set(first_edge) == node,
                 "neural_new: index sets of bias and first_edge do not agree") /\
          forall(i in index_set(input_ids))
    (assert(input_ids[i] in node,
                 "neural_net: input_ids\(i) = \(input_ids[i]) not a correct node number"))
    /\
          forall(i in index_set(output_ids))
    (assert(output_ids[i] in node,
                 "neural_net: output_ids\(i) = \(output_ids[i]) not a correct node number"))
    /\
          forall(i in index_set(first_edge))
    (assert(first_edge[i] in edge,
                 "neural_net: first_edge\(i) = \(first_edge[i]) not a correct edge number"))
    /\
          forall(i in index_set(edge_parent))
    (assert(edge_parent[i] in edge,
                 "neural_net: edge_parent\(i) = \(edge_parent[i]) not a correct node number"))
    /\
    assert(mincreasing(first_edge),
                 "neural_net: first_edge array is not in increasing order")
    /\
          fzn_neural_net(inputs,input_ids,outputs,output_ids,bias,
                         edge_weight,edge_parent,first_edge,neuron_type);

test mincreasing(array[int] of int: x) =
    forall(i in index_set(x) diff { max(index_set(x)) })
          (x[i] <= x[i+1]);

predicate neural_net(array[int] of var float: inputs,
                     array[int] of int: input_ids,
                     array[int] of var float: outputs,
                     array[int] of int: output_ids,
                     array[int] of float: bias,
                     array[int] of float: edge_weight,
                     array[int] of int: edge_parent,
                     array[int] of int: first_edge) =
    neural_net(inputs,input_ids,outputs,output_ids,bias,
               edge_weight,edge_parent,first_edge,NT_RELU);

%-----------------------------------------------------------------------------%

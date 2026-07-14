include "fzn_neural_net.mzn";
include "fzn_neural_net_reif.mzn";

enum NEURON_TYPE = {NT_RELU, NT_STEP, NT_LINEAR, NT_SOFTPLUS};

predicate fzn_neural_net(
  array [int] of var float: inputs,
  array [int] of int: input_ids,
  array [int] of var float: outputs,
  array [int] of int: output_ids,
  array [int] of float: biases,
  array [int] of float: edge_weights,
  array [int] of int: edge_parents,
  array [int] of int: first_edges,
  NEURON_TYPE: neuron_type,
);

/** @group globals.learning
  Constrain the output layer of a neural net to take the value defined by the input layer.

  @param inputs: an array of float variables
  @param input_ids: array[int] of node
  @param outputs: an array of float variables
  @param output_ids: array[int] of node
  @param biases: array[node] of float
  @param edge_weights: array[edge] of float
  @param edge_parents: array[edge] of neuron (start neuron for edge)
  @param first_edges: array[node] of 1..m+1
  @param neuron_type: { NT_RELU, NT_STEP, NT_LINEAR, NT_SOFTPLUS }
*/
predicate neural_net(
  array [int] of var float: inputs,
  array [int] of int: input_ids,
  array [int] of var float: outputs,
  array [int] of int: output_ids,
  array [int] of float: biases,
  array [int] of float: edge_weights,
  array [int] of int: edge_parents,
  array [int] of int: first_edges,
  NEURON_TYPE: neuron_type,
) =
  assert(
    index_set(inputs) == index_set(input_ids),
    "neural_net: number of input vars not equal to number of input_ids",
  ) /\
    assert(
      index_set(outputs) == index_set(output_ids),
      "neural_net: number of output vars not equal to number of output_ids",
    ) /\
    (
      let {
        set of int: node = index_set(biases);
      } in let {
        set of int: edge = index_set(edge_weights);
      } in assert(
        index_set(edge_parents) == edge,
        "neural_net: index sets of edge_weights and edge_parents do not agree",
      ) /\
        assert(
          index_set(first_edges) == node,
          "neural_new: index sets of biases and first_edges do not agree",
        ) /\
        forall (i in index_set(input_ids)) (
          assert(
            input_ids[i] in node,
            "neural_net: input_ids\(i) = \(input_ids[i]) not a correct node number",
          )
        ) /\
        forall (i in index_set(output_ids)) (
          assert(
            output_ids[i] in node,
            "neural_net: output_ids\(i) = \(output_ids[i]) not a correct node number",
          )
        ) /\
        forall (i in index_set(first_edges)) (
          assert(
            first_edges[i] in edge,
            "neural_net: first_edges\(i) = \(first_edges[i]) not a correct edge number",
          )
        ) /\
        forall (i in index_set(edge_parents)) (
          assert(
            edge_parents[i] in edge,
            "neural_net: edge_parents\(i) = \(edge_parents[i]) not a correct node number",
          )
        ) /\
        assert(
          mincreasing(first_edges),
          "neural_net: first_edges array is not in increasing order",
        ) /\
        fzn_neural_net(
          inputs,
          input_ids,
          outputs,
          output_ids,
          biases,
          edge_weights,
          edge_parents,
          first_edges,
          neuron_type,
        )
    );

test mincreasing(array [int] of int: x) =
  forall (i in index_set(x) diff {max(index_set(x))}) (x[i] <= x[i + 1]);

predicate neural_net(
  array [int] of var float: inputs,
  array [int] of int: input_ids,
  array [int] of var float: outputs,
  array [int] of int: output_ids,
  array [int] of float: biases,
  array [int] of float: edge_weights,
  array [int] of int: edge_parents,
  array [int] of int: first_edges,
) =
  neural_net(
    inputs,
    input_ids,
    outputs,
    output_ids,
    biases,
    edge_weights,
    edge_parents,
    first_edges,
    NT_RELU,
  );

%-----------------------------------------------------------------------------%

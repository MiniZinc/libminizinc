predicate fzn_neural_net_reif(array[int] of var float: inputs,
                         array[int] of int: input_ids,
		                     array[int] of var float: outputs,
		                     array[int] of int: output_ids,
		                     array[int] of float: bias,
		                     array[int] of float: edge_weight,
		                     array[int] of int: edge_parent,
		                     array[int] of int: first_edge,
	                       NEURON_TYPE: neuron_type,
                         var bool: b) =
  abort("Reification of neural_net constraint is not supported");

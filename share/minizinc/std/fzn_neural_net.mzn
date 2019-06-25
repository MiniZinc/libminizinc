predicate fzn_neural_net(array[int] of var float: inputs,
                         array[int] of int: input_ids,
		                     array[int] of var float: outputs,
		                     array[int] of int: output_ids,
		                     array[int] of float: bias,
		                     array[int] of float: edge_weight,
		                     array[int] of int: edge_parent,
		                     array[int] of int: first_edge,
	                       NEURON_TYPE: neuron_type) =		
    let { set of int: NODE = index_set(bias) } in
	  let { set of int: INPUTS = array2set(input_ids) } in
    let { set of int: EDGE = index_set(edge_weight) } in
    let { array[NODE] of var float: neuron } in
        forall(i in index_set(inputs))(neuron[input_ids[i]] = inputs[i]) /\
        forall(i in index_set(outputs))(neuron[output_ids[i]] = outputs[i]) /\
        forall(i in NODE diff INPUTS) (
            let { int: first = first_edge[i];
                  int: last = if i = max(NODE) then max(index_set(first_edge))
                              else first_edge[i+1] - 1 endif;
                  array[int] of var float: ins = [neuron[edge_parent[j]] | j in first..last];
		              array[int] of float: ws = [ edge_weight[j] | j in first..last ];
	                float: b = bias[i]; } in
                      neuron[i] = if neuron_type = NT_RELU then neuron_relu(ins,ws,b)
                                  elseif neuron_type = NT_STEP then neuron_step(ins,ws,b)
                                  elseif neuron_type = NT_LINEAR then neuron_linear(ins,ws,b)
                                  elseif neuron_type = NT_SOFTPLUS then neuron_softplus(ins,ws,b)
			                            else 0.0 endif
        );
                       
%-----------------------------------------------------------------------------%

function var float: neuron_relu(array[int] of var float: inputs,
	                 array[int] of float: weights,
	                 float: bias) =
         max(0.0, sum(i in index_set(inputs))(weights[i]*inputs[i]) + bias);
         
function var float: neuron_step(array[int] of var float: inputs,
	                 array[int] of float: weights,
	                 float: bias) =
         (sum(i in index_set(inputs))(weights[i]*inputs[i]) + bias >= 0.0);
         
function var float: neuron_linear(array[int] of var float: inputs,
	                 array[int] of float: weights,
	                 float: bias) =
         (sum(i in index_set(inputs))(weights[i]*inputs[i]) + bias);
         
function var float: neuron_softplus(array[int] of var float: inputs,
	                 array[int] of float: weights,
	                 float: bias) =
         (ln(1 + exp(sum(i in index_set(inputs))(weights[i]*inputs[i]) + bias)));

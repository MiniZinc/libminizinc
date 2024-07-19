include "experimental/blackbox/fzn_blackbox.mzn";
include "experimental/blackbox/fzn_blackbox_bounds.mzn";

/** @group blackbox.annotations
  Define the propagator to be executed by the given \a executable, passing the
  given command line \a args.
*/
annotation blackbox_exec(string: executable, list of string: args);
/** @group blackbox.annotations
  Define the propagator to be executed by the given shared \a library, passing
  the given \a args to it.
*/
annotation blackbox_dll(string: library, list of string: args);

% Convenience forms without arguments (rewritten into the form with an empty argument list).
function ann: blackbox_exec(string: executable) = blackbox_exec(executable, []);
function ann: blackbox_dll(string: library) = blackbox_dll(library, []);

% Internal builtin: resolve the executable (`is_library=false`) or shared library
% (`is_library=true`) `name` to an absolute path on the compilation host. If it
% cannot be located, `name` is returned unchanged and a warning is issued. The
% compiler wraps the source-annotation target in a call to this function.
function string: mzn_resolve_blackbox_source(string: name, bool: is_library);

/** @group blackbox.annotations
  Mark a bodyless predicate or function as a value propagator. The MiniZinc
  compiler generates its body as a call to the generic `blackbox` constraint.
  A `predicate` acts as a relational checker, a `function` returning a list of
  variables computes an output value.
  The propagator must also carry a `blackbox_exec` or `blackbox_dll` annotation.
*/
annotation minizinc_value_propagator;
/** @group blackbox.annotations
  Mark a bodyless predicate as a bounds propagator. The MiniZinc compiler
  generates its body as a call to the generic `blackbox_bounds` constraint.
  The propagator must also carry a `blackbox_exec` or `blackbox_dll` annotation.
*/
annotation minizinc_bounds_propagator;

/** @group blackbox.blackbox
  Instruct the solver to run a blackbox function when all decision variable in \a int_input and \a float_input are fixed, binding the output of the function to \a int_output and \a float_output.

  The blackbox function to use must be defined using the annotation `blackbox_exec` or `blackbox_dll`.
*/
predicate blackbox(
	list of var int: int_input,
	list of var float: float_input,
	list of var int: int_output,
	list of var float: float_output,
) = fzn_blackbox(
	int_input,
	float_input,
	int_output,
	float_output,
);

enum PropBnd = {PR_LB, PR_UB};

% Serialise a list of bound literals into the flat integer wire format:
% [ number of literals, (variable index, bound)... ].
function list of int: blackbox_flatten_literals(
	list of tuple(int, PropBnd): literals,
) = [length(literals)] ++ [
	if j == 0 then literals[k].1 else enum2int(literals[k].2) endif
| k in index_set(literals), j in 0..1 ];

/** @group blackbox.blackbox_bounds
  Instruct the solver to run a blackbox function if any bounds of the decision variable in \a int_input or \a float_input change, to compute new bounds for the decision variables.
  The blackbox function is provided with a lower bound and upper bound for each decision variable, and must return a lower bound and upper bound for each decision variable in the same order.
	\a reason provides a blueprint reason for any bounds change: for each decision variable it gives its index together with the literals that explain, respectively, its lower and upper bound change.

  The blackbox function to use must be defined using the annotation `blackbox_exec` or `blackbox_dll`.
*/
predicate blackbox_bounds(
	list of var int: int_input,
	list of var float: float_input,
	list of tuple(int, list of tuple(int, PropBnd), list of tuple(int, PropBnd)): reason,
)  = let {
	int: num_var = length(int_input) + length(float_input);
	constraint assert(length(reason) == num_var, "blackbox_bounds: expected \(num_var) reason descriptions for the \(num_var) decision variables, but \(length(reason)) reason blueprints provided");
	list of int: flat_reason = [
		v
	| entry in reason,
		v in [entry.1] ++ blackbox_flatten_literals(entry.2) ++ blackbox_flatten_literals(entry.3)
	];
} in fzn_blackbox_bounds(
	int_input,
	float_input,
	flat_reason,
);

/** @group blackbox.blackbox_bounds
  Conservative default reason for a bounds propagator over \a n decision
  variables: every bound of every input is assumed to explain every output
  bound. Used when a propagator does not define its own reason function.
*/
function list of tuple(int, list of tuple(int, PropBnd), list of tuple(int, PropBnd)): blackbox_default_reason(int: n) =
	let {
		list of tuple(int, PropBnd): all_literals = [(j, b) | j in 1..n, b in PropBnd];
	} in [(i, all_literals, all_literals) | i in 1..n];

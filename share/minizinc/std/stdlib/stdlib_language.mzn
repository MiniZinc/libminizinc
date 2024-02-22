/***
  @groupdef stdlib.language Language information

  These functions return information about the MiniZinc system.
*/

/** @group stdlib.language Return MiniZinc version encoded as an integer (major*10000+minor*1000+patch). */
function int: mzn_compiler_version();

/** @group stdlib.language Return string representation of \a v given an integer major*10000+minor*1000+patch
*/
function string: mzn_version_to_string(int: v) =
  show(v div 10000)++"."++show((v div 1000) mod 10)++"."++show(v mod 100);

/** @group stdlib.language If defined, this can be used to check that the MiniZinc compiler supports all the features used in the model. */
opt int: mzn_min_version_required;

constraint assert(absent(mzn_min_version_required) \/ deopt(mzn_min_version_required) <= mzn_compiler_version(), "This model requires MiniZinc version "++mzn_version_to_string(deopt(mzn_min_version_required))++" but you are running version "++mzn_version_to_string(mzn_compiler_version()));

/***
 @groupdef stdlib.options Compiler options
*/

% TODO: Is this still in use?
/** @group stdlib.options Whether to only generate domains that are contiguous ranges */
opt bool: mzn_opt_only_range_domains;

/** @group stdlib.options Check whether to only generate domains that are contiguous ranges */

test mzn_check_only_range_domains() =
  if absent(mzn_opt_only_range_domains) then false
  else deopt(mzn_opt_only_range_domains) endif;

/** @group stdlib.options Whether to generate defines_var annotations */
opt bool: mzn_opt_annotate_defines_var;

/** @group stdlib.options Check whether to generate defines_var annotations */
test mzn_check_annotate_defines_var() =
  if absent(mzn_opt_annotate_defines_var) then true
  else deopt(mzn_opt_annotate_defines_var) endif;

/** @group stdlib.options Whether to generate computed_domain annotations */
opt bool: mzn_opt_annotate_computed_domains;

/** @group stdlib.options Check whether to generate computed_domain annotations */
test mzn_check_annotate_computed_domains() =
  if absent(mzn_opt_annotate_computed_domains) then false
  else deopt(mzn_opt_annotate_computed_domains) endif;

/** @group stdlib.options Whether to ignore symmetry breaking constraints
 
    If not specified or set to false, it depends on the solver library whether
    constraints that are wrapped in a symmetry_breaking_constraint call
    are in fact compiled. If set to true, they are not compiled, independent
    of the solver library.
 */
opt bool: mzn_ignore_symmetry_breaking_constraints;

/** @group stdlib.options Check whether to ignore symmetry breaking constraints */
test mzn_check_ignore_symmetry_breaking_constraints() =
  if absent(mzn_ignore_symmetry_breaking_constraints) then false
  else deopt(mzn_ignore_symmetry_breaking_constraints) endif;

/** @group stdlib.options Whether to ignore redundant constraints
  
    If not specified or set to false, it depends on the solver library whether
    constraints that are wrapped in a redundant_constraint call
    are in fact compiled. If set to true, they are not compiled, independent
    of the solver library.
 */
opt bool: mzn_ignore_redundant_constraints;

/** @group stdlib.options Check whether to ignore redundant constraints */
test mzn_check_ignore_redundant_constraints() =
  if absent(mzn_ignore_redundant_constraints) then false
  else deopt(mzn_ignore_redundant_constraints) endif;


/** @group stdlib.options Whether to use zero as the underlying representation
    for absent optional variables.
  
    If not specified or set to true, zero is used as the underlying
    representation for absent optional variables, breaking symmetry. If set to
    false, the underlying representation of absent optional variables is left
    unconstrained.
 */
opt bool: mzn_absent_zero;

/** @group stdlib.options Check whether to use zero as the underlying
    representation of absent optional variables */
test mzn_check_absent_zero() =
  if absent(mzn_absent_zero) then true
  else deopt(mzn_absent_zero) endif;

/** @group stdlib.options Whether to half reify bool_clause constraints */
opt bool: mzn_half_reify_clause;

/** @group stdlib.options Check to half reify bool_clause constraints  */
test mzn_check_half_reify_clause() =
  if absent(mzn_half_reify_clause) then true
  else deopt(mzn_half_reify_clause) endif;

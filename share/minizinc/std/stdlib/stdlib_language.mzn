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

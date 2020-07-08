%-----------------------------------------------------------------------------%
% MiniZinc standard library.
%-----------------------------------------------------------------------------%
% This file contains declarations of all functions, predicates and annotations
% available in the base MiniZinc language.

/***
 @groupdef MAIN The MiniZinc library
*/

%-----------------------------------------------------------------------------%
%
% FlatZinc builtins
%
%  This section contains declarations for the standard FlatZinc builtins.
%  They can be redefined by providing a custom redefinitions.mzn in the
%  solver globals library.
%

include "flatzinc_builtins.mzn";

include "redefinitions.mzn";
include "redefinitions-2.0.mzn";
include "redefinitions-2.0.2.mzn";
include "redefinitions-2.1.mzn";
include "redefinitions-2.1.1.mzn";
include "redefinitions-2.2.1.mzn";
include "redefinitions-2.3.3.mzn";

/***
  @groupdef stdlib Standard Library

  These functions and predicates define built-in operations of the MiniZinc language.

*/

include "stdlib_language.mzn";
include "stdlib_ann.mzn";
include "stdlib_opt.mzn";
include "stdlib_options.mzn";
include "stdlib_compare.mzn";
include "stdlib_math.mzn";
include "stdlib_logic.mzn";
include "stdlib_set.mzn";
include "stdlib_ite.mzn";
include "stdlib_array.mzn";
include "stdlib_sort.mzn";
include "stdlib_coercion.mzn";
include "stdlib_string.mzn";
include "stdlib_reflect.mzn";
include "stdlib_debug.mzn";
include "stdlib_enum.mzn";
include "stdlib_random.mzn";

% Undocumented internal implementation used during compilation
include "stdlib_internal.mzn";


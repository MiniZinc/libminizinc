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
%  This section contains declarations for the standard FlatZinc builtins.  They
%  can be redefined by providing a custom redefinitions.mzn in the solver
%  globals library. A standard redefinition is provided for FlatZinc builtins
%  after version 2.0. This redefinition can be overriden by providing a custom
%  redefinitions-<VERSION>.mzn

include "flatzinc_builtins.mzn";

/***
  @groupdef stdlib Standard Library

  These functions and predicates define built-in operations of the MiniZinc language.

*/

include "stdlib/stdlib_language.mzn";
include "stdlib/stdlib_ann.mzn";
include "stdlib/stdlib_opt.mzn";
include "stdlib/stdlib_compare.mzn";
include "stdlib/stdlib_math.mzn";
include "stdlib/stdlib_logic.mzn";
include "stdlib/stdlib_set.mzn";
include "stdlib/stdlib_ite.mzn";
include "stdlib/stdlib_array.mzn";
include "stdlib/stdlib_sort.mzn";
include "stdlib/stdlib_coercion.mzn";
include "stdlib/stdlib_string.mzn";
include "stdlib/stdlib_reflect.mzn";
include "stdlib/stdlib_debug.mzn";
include "stdlib/stdlib_enum.mzn";
include "stdlib/stdlib_random.mzn";

% Undocumented internal implementation used during compilation
include "stdlib/stdlib_internal.mzn";


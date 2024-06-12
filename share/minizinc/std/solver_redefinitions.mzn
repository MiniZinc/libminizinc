%-----------------------------------------------------------------------------%
% Solver redefinitions
%-----------------------------------------------------------------------------%
% This file contains all files that can be used by solver libraries to provide
% or override redefinitions of FlatZinc builtins.

include "redefinitions.mzn";
include "redefinitions-2.0.mzn";
include "redefinitions-2.0.2.mzn";
include "redefinitions-2.1.mzn";
include "redefinitions-2.1.1.mzn";
include "redefinitions-2.2.1.mzn";
include "redefinitions-2.3.3.mzn";
include "redefinitions-2.5.2.mzn";
include "redefinitions-2.7.1.mzn";

% Inclusion of count constraints for internal redefinitions of hidden counting constraints
include "count_eq.mzn";
include "count_geq.mzn";
include "count_leq.mzn";
include "count_neq.mzn";
include "count_gt.mzn";
include "count_lt.mzn";
include "count_fn.mzn";
include "fzn_if_then_else_int.mzn";
include "fzn_if_then_else_opt_int.mzn";
include "fzn_if_then_else_var_int.mzn";
include "fzn_if_then_else_var_opt_int.mzn";
include "fzn_if_then_else_bool.mzn";
include "fzn_if_then_else_opt_bool.mzn";
include "fzn_if_then_else_var_bool.mzn";
include "fzn_if_then_else_var_opt_bool.mzn";
include "fzn_if_then_else_float.mzn";
include "fzn_if_then_else_opt_float.mzn";
include "fzn_if_then_else_var_float.mzn";
include "fzn_if_then_else_var_opt_float.mzn";
include "fzn_if_then_else_set.mzn";
include "fzn_if_then_else_var_set.mzn";
include "fzn_if_then_else_partiality.mzn";
include "lex_less.mzn";
include "lex_lesseq.mzn";
include "fzn_array_int_union.mzn";
include "fzn_array_opt_int_union.mzn";
include "fzn_array_set_union.mzn";
include "fzn_array_set_intersect.mzn";
include "table.mzn";

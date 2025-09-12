% This file contains redefinitions of standard builtins for version 2.1.1
% that can be overridden by solvers.

function var $$E: min(
  var set of $$E: xs,
) =
  let {
    var set of int: s = enum2int(xs);
    var min(ub(s))..max(ub(s)): m = min([e + (max(ub(s)) + 1 - e) * (1 - (e in s)) | e in ub(s)]);
  } in to_enum_internal(enum_of(xs), m);

%% The following can be used as an alternative if the solver supports
%% a FlatZinc builtin set_min:

% predicate set_min(var set of int: s, var int: m);
% 
% function var $$E: min(var set of $$E: s) =
%   if mzn_in_root_context() then min_t(s) else
%   let { constraint card(s) > 0 } in min_mt(s) endif;
% 
% function var $$E: min_t(var set of $$E: s) ::promise_total =
%   let {
%     var min(ub(s))..max(ub(s)): ms ::is_defined_var;
%     constraint card(s) > 0;
%     constraint set_min(s,ms) ::defines_var(ms);
%   } in ms;
% 
% function var $$E: min_mt(var set of $$E: s) ::promise_total =
%   let {
%     var set of ub(s) union {1}: x;
%     var bool: b = card(s) > 0;
%     constraint b -> x=s;
%     constraint b \/ 1 in x;
%   } in min_t(x);

function var $$E: max(
  var set of $$E: xs,
) =
  let {
    var set of int: s = enum2int(xs);
    var min(ub(s))..max(ub(s)): m = max([e + (min(ub(s)) - 1 - e) * (1 - (e in s)) | e in ub(s)]);
  } in to_enum_internal(enum_of(xs), m);

%% The following can be used as an alternative if the solver supports
%% a FlatZinc builtin set_max:

% predicate set_max(var set of int: s, var int: m);
% 
% function var $$E: max(var set of $$E: s) =
%   if mzn_in_root_context() then max_t(s) else
%   let { constraint card() > 0 } in max_mt(s) endif;
% 
% function var $$E: max_t(var set of $$E: s) ::promise_total =
%   let {
%     var min(ub(s))..max(ub(s)): ms ::is_defined_var;
%     constraint card(s) > 0;
%     constraint set_max(s,ms) ::defines_var(ms);
%   } in ms;
% 
% function var $$E: max_mt(var set of $$E: s) ::promise_total =
%   let {
%     var set of ub(s) union {1}: x;
%     var bool: b = card(s) > 0;
%     constraint b -> x=s;
%     constraint b \/ 1 in x;
%   } in max_t(x);

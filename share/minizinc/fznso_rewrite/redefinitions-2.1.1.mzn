% Shadows `std`'s file of the same name, carrying it over unchanged.
%
% The copy exists so that the overlay owns every `redefinitions` file, and a
% solver shipping one cannot quietly take these with it.
%
% `min` and `max` over a set variable are *not* here. `std` writes them as the
% extremum of an array of scaled indicators, which is right for a solver holding
% the set itself but costs a row per member for one decomposing it to bits. They
% live in `fznso_set_functions.mzn` instead, which each path already overrides:
% `fznso_constraints/` keeps `std`'s form, `fznso_nosets/` states them over the
% bits directly.


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

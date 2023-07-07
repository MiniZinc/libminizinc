include "experimental/on_restart/fzn_on_restart_complete.mzn";
include "experimental/on_restart/fzn_on_restart_last_val_bool.mzn";
include "experimental/on_restart/fzn_on_restart_last_val_float.mzn";
include "experimental/on_restart/fzn_on_restart_last_val_int.mzn";
include "experimental/on_restart/fzn_on_restart_last_val_set.mzn";
include "experimental/on_restart/fzn_on_restart_sol_bool.mzn";
include "experimental/on_restart/fzn_on_restart_sol_float.mzn";
include "experimental/on_restart/fzn_on_restart_sol_int.mzn";
include "experimental/on_restart/fzn_on_restart_sol_set.mzn";
include "experimental/on_restart/fzn_on_restart_status.mzn";
include "experimental/on_restart/fzn_on_restart_uniform_float.mzn";
include "experimental/on_restart/fzn_on_restart_uniform_int.mzn";

/***
  @groupdef experimental.on_restart On Restart

  These annotations and functions provide the basic building blocks to implement
  meta-heuristics triggered on restart and governed search with the use of
  solver internal values and functionality.
*/

/**
  @group experimental.on_restart A search annotation that triggers a function
  to be executed on restart
*/
ann: on_restart(string: pred);

/**
  @group experimental.on_restart Reports the status of the solver (before
  restarting).
*/
enum STATUS = {START, UNKNOWN, UNSAT, SAT, OPT};
function var STATUS: status() ::promise_total = let {
  var STATUS: ret;
  constraint fzn_on_restart_status(ret);
} in ret;


/**
  @group experimental.on_restart When complete is set to 'true', then it
  marks the search as complete.
*/
predicate complete() = complete_reif(true);
predicate complete_reif(var bool: marker) = fzn_on_restart_complete(marker);

/**
  @group experimental.on_restart Returns a new random value between \a low
  and \a high chosen from a uniform distribution whenver the solver restarts.
*/
function var $$E: uniform_on_restart($$E: low, $$E: high) ::no_cse ::promise_total = let {
  var low..high: ret;
  constraint fzn_on_restart_uniform_int(low, high, ret);
} in ret;

/**
  @group experimental.on_restart Returns a new random value from the set \a S
  chosen from a uniform distribution whenver the solver restarts.
*/
function var $$E: uniform_on_restart(set of $$E: S) ::no_cse =
  if card(S) == max(S) - min(S) + 1 then
    uniform_on_restart(min(S), max(S))
  else
    [ i | i in S ][uniform_on_restart(1, card(S))]
  endif;

/**
  @group experimental.on_restart Returns a new random value between \a low
  and \a high chosen from a uniform distribution whenver the solver restarts.
*/
function var float: uniform_on_restart(float: low, float: high) ::no_cse ::promise_total = let {
  var low..high: ret;
  constraint fzn_on_restart_uniform_float(low, high, ret);
} in ret;

/**
  @group experimental.on_restart Provides access to last values taken by a model
  variables. The last_val functions are only safe to use when the STATUS has
  been SAT or OPT.
*/
function var bool: last_val(var bool: x) ::promise_total =
  if is_fixed(x) then
    fix(x)
  else
    let {
      var bool: ret;
      constraint fzn_on_restart_last_val_bool(x, ret);
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to last values taken by a model
  variables. The last_val functions are only safe to use when the STATUS has
  been SAT or OPT.
*/
function var float: last_val(var float: x) ::promise_total  =
  if is_fixed(x) then
    fix(x)
  else
    let {
      var lb(x)..ub(x): ret;
      constraint fzn_on_restart_last_val_float(x, ret);
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to last values taken by a model
  variables. The last_val functions are only safe to use when the STATUS has
  been SAT or OPT.
*/
function var $$E: last_val(var $$E: x) ::promise_total  =
  if is_fixed(x) then
    fix(x)
  else
    let {
      var dom(x): ret;
      constraint fzn_on_restart_last_val_int(x, ret);
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to last values taken by a model
  variables. The last_val functions are only safe to use when the STATUS has
  been SAT or OPT.
*/
function var opt $$E: last_val(var opt $$E: x) ::promise_total  =
  if is_fixed(x) then
    fix(x)
  elseif is_fixed(occurs(x)) then
    if fix(occurs(x)) then
      last_val(deopt(x))
    else
      <>
    endif
  else
    let {
      var opt dom(x): ret;
      constraint occurs(ret) = last_val(occurs(x));
      constraint deopt(ret) = last_val(deopt(x));
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to last values taken by a model
  variables. The last_val functions are only safe to use when the STATUS has
  been SAT or OPT.
*/
function var set of $$E: last_val(var set of $$E: x) ::promise_total  =
  if is_fixed(x) then
    fix(x)
  else
    let {
      var set of ub(x): ret;
      constraint fzn_on_restart_last_val_set(x, ret);
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to solution values of model
  variables. The sol function are only safe to use when the STATUS has been SAT
  or OPT.
*/
function var bool: sol(var bool: x) ::promise_total =
  if is_fixed(x) then
    fix(x)
  else
    let {
      var bool: ret;
      constraint fzn_on_restart_sol_bool(x, ret);
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to solution values of model
  variables. The sol function are only safe to use when the STATUS has been SAT
  or OPT.
*/
function var float: sol(var float: x) ::promise_total =
  if is_fixed(x) then
    fix(x)
  else
    let {
      var lb(x)..ub(x): ret;
      constraint fzn_on_restart_sol_float(x, ret);
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to solution values of model
  variables. The sol function are only safe to use when the STATUS has been SAT
  or OPT.
*/
function var $$E: sol(var $$E: x) ::promise_total =
  if is_fixed(x) then
    fix(x)
  else
    let {
      var dom(x): ret;
      constraint fzn_on_restart_sol_int(x, ret);
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to solution values of model
  variables. The sol function are only safe to use when the STATUS has been SAT
  or OPT.
*/
function var opt $$E: sol(var opt $$E: x) ::promise_total  =
  if is_fixed(x) then
    fix(x)
  elseif is_fixed(occurs(x)) then
    if fix(occurs(x)) then
      sol(deopt(x))
    else
      <>
    endif
  else
    let {
      var opt dom(x): ret;
      constraint occurs(ret) = sol(occurs(x));
      constraint deopt(ret) = sol(deopt(x));
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to solution values of model
  variables. The sol function are only safe to use when the STATUS has been SAT
  or OPT.
*/
function var set of $$E: sol(var set of $$E: x) ::promise_total =
  if is_fixed(x) then
    fix(x)
  else
    let {
      var set of ub(x): ret;
      constraint fzn_on_restart_sol_set(x, ret);
    } in ret
  endif;

/**
  @group experimental.on_restart Provides access to solution values of model
  variables. The sol function are only safe to use when the STATUS has been SAT
  or OPT.
*/
function array[$$E] of var bool: sol(array[$$E] of var bool: x) =
  [i: sol(x[i]) | i in index_set(x)];

/**
  @group experimental.on_restart Provides access to solution values of model
  variables. The sol function are only safe to use when the STATUS has been SAT
  or OPT.
*/

function array[$$E] of var float: sol(array[$$E] of var float: x) =
  [i: sol(x[i]) | i in index_set(x)];

/**
  @group experimental.on_restart Provides access to solution values of model
  variables. The sol function are only safe to use when the STATUS has been SAT
  or OPT.
*/
function array[$$E] of var $$F: sol(array[$$E] of var $$F: x) =
  [i: sol(x[i]) | i in index_set(x)];


/**
  @group experimental.on_restart round_robin provides a metaheuristic for LNS
  where each neighbourhood is chosen sequentially.
*/
predicate round_robin(array[int] of var bool: nbhs) =
  let {
    int: len = length(nbhs);
    % Transposed neighbourhoods in case of index set not starting from 1
    array[1..len] of var bool: t_nbhs = [ bv | bv in nbhs];
    % Neighbourhood selection
    var -1..len-1: select;
  } in forall(i in 1..len) ( (select-1 == i) -> t_nbhs[i]) /\
    if status() == START then
      select = -1
    else
      select = (last_val(select) + 1) mod len
    endif;

/**
  @group experimental.on_restart 'basic_lns' provides LNS in its simplest form
  where one nbh is applied on every restart.
*/
predicate basic_lns(var bool: nbh) =
  (status() != START) -> nbh;

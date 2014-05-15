%-----------------------------------------------------------------------------%
% MiniZinc standard library.
%-----------------------------------------------------------------------------%
% This file contains built-in operations that can be expressed in MiniZinc,
% and so are not implemented as true built-ins within the compiler.

%-----------------------------------------------------------------------------%
%
% General annotations
%

annotation promise_total;
annotation is_defined_var;
annotation var_is_introduced;
annotation defines_var(var $t: c);
annotation output_array(array[$u] of set of int:a);
annotation output_var;
annotation is_reverse_map;

%-----------------------------------------------------------------------------%
%
% Propagation strength
%

annotation domain;
annotation bounds;


%-----------------------------------------------------------------------------%
%
% Search annotations
%

annotation seq_search(array[int] of ann: s);

annotation int_search(
    array[int] of var int: vars,
    ann: select,
    ann: choice,
    ann: explore,
);

annotation int_search_all(
    ann: select,
    ann: choice,
    ann: explore
);

annotation int_search_bd(
    ann: a,
    ann: b,
    ann: c,
    ann: d
);

annotation bool_search(
    array[int] of var bool: vars,
    ann: select,
    ann: choice,
    ann: explore
);

annotation float_search(
    array[int] of var float: vars,
    float: prec,
    ann: select,
    ann: choice,
    ann: explore
);

annotation set_search(
    array[int] of var set of int: vars,
    ann: select,
    ann: choice,
    ann: explore
);

annotation labelling_ff;

%-----------------------------------------------------------------------------%
%
% Variable selection annotations.
%

annotation input_order;
annotation first_fail;
annotation anti_first_fail;
annotation smallest;
annotation largest;
annotation occurrence;
annotation most_constrained;
annotation max_regret;
annotation dom_w_deg;
annotation impact;

%-----------------------------------------------------------------------------%
%
% Domain reduction strategies.
%

annotation indomain;
annotation indomain_min;
annotation indomain_max;
annotation indomain_middle;
annotation indomain_median;
annotation indomain_random;
annotation indomain_split;
annotation indomain_split_random;
annotation indomain_reverse_split;
annotation indomain_interval;
annotation outdomain_min;
annotation outdomain_median;
annotation outdomain_random;

%-----------------------------------------------------------------------------%
%
% Exploration strategies.
%

annotation complete;
annotation bbs(int: s);
annotation fail;
annotation lds(int: d);
annotation credit(int: c, ann: explore);
annotation dbs(int: d, ann: explore);
annotation restart_geometric(float: i, float: s, ann: search);
annotation limit(ann: measure, int: value, ann: search);
annotation time;

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

%-----------------------------------------------------------------------------%
%
% Option type predicates.
%


predicate deopt(var opt bool: b) ::promise_total =
  let { var bool: bo; } in bo;
predicate occurs(var opt bool: b) ::promise_total =
  let { var bool: bo; } in bo;
  
predicate deopt(var bool: b) = b;
predicate occurs(var bool: b) = true;

predicate absent(var opt bool: b) = not occurs(b);
predicate absent(var bool: b) = false;

predicate bool_eq(var opt bool: b0, var opt bool: b1) =
     (absent(b0) /\ absent(b1))
  \/ (occurs(b0) /\ occurs(b1) /\ deopt(b0)=deopt(b1));

predicate bool_eq(var opt bool: b0, var bool: b1) =
  occurs(b0) /\ deopt(b0)=b1;

predicate bool_eq(var bool: b0, var opt bool: b1) =
  occurs(b1) /\ deopt(b1)=b0;

predicate forall (array[int] of var opt bool: x) =
  forall ([absent(x[i]) \/ deopt(x[i]) | i in index_set(x)]);

predicate exists (array[int] of var opt bool: x) =
  exists ([absent(x[i]) \/ deopt(x[i]) | i in index_set(x)]);

function bool: occurs(var int: x) ::promise_total = true;
function var int: deopt(var int: x) ::promise_total = x;

function bool: absent(var int: x) ::promise_total = false;
function var bool: absent(var opt int: x) ::promise_total = not occurs(x);

function var bool: occurs_internal(var opt int: x) ::promise_total =
  let { var bool : b; } in b;
function var int : deopt_internal(var opt int : x) ::promise_total =
  let { var lb(x)..ub(x): y } in y;

function var opt int: reverse_map(var bool: occ, var int: d);
function int: reverse_map(bool: occ, int: d) ::promise_total =
  if occ then d else 0 endif;

function var bool : occurs(var opt int : x) ::promise_total =
  let {
    var bool : b = occurs_internal(x);
    var int  : dx = deopt_internal(x);
    constraint (x = reverse_map(b,dx)) :: is_reverse_map;
  } in b;
  
function var int : deopt(var opt int : x) ::promise_total =
  let { var lb(x)..ub(x): y } in y;

predicate int_eq(var opt int : x, var opt int : y) =
     (absent(x) /\ absent(y))
  \/ (occurs(x) /\ occurs(y) /\ deopt(x)=deopt(y));

function var int: sum(array[int] of var opt int: x) =
  sum (i in index_set(x)) (let { var int: dx = deopt(x[i]) } in if occurs(x[i]) then dx else 0 endif);

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

% Include solver-specific redefinitions for any FlatZinc built-ins.
%
include "redefinitions.mzn"

%-----------------------------------------------------------------------------%
%-----------------------------------------------------------------------------%

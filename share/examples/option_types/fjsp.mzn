%-----------------------------------------------------------------------------%
% vim: ts=4 sw=4 et wm=0 tw=0
%-----------------------------------------------------------------------------%
% Copyright (C) 2013 National ICT Australia.
% See the file COPYING for license information.
%-----------------------------------------------------------------------------%
%
% Author(s): 
%   Andreas Schutt <andreas.schutt@nicta.com.au>
%
%-----------------------------------------------------------------------------%
% Flexible Job Shop Scheduling
%
% Flexible Job Shop Scheduling (FJSS) is more general version of Job Shop
% Scheduling in which some tasks can be run an alternative machines. The goal
% is to find a feasible schedule minimising the makespan.
%
%-----------------------------------------------------------------------------%

% This model uses option types. The weak <= and = predicates should become part
% of the standard library.

%-----------------------------------------------------------------------------%
% Parameters

int: no_mach;   % Number of machines
int: no_jobs;   % Number of jobs
int: no_task;   % Number of total tasks
int: no_optt;   % Number of total optional tasks

set of int: Mach  = 1..no_mach;
set of int: Jobs  = 1..no_jobs;
set of int: Tasks = 1..no_task;
set of int: OptTs = 1..no_optt;

array [Jobs] of set of int: tasks;
array [Tasks] of set of int: optts;

array [OptTs] of int: optt_mach;
array [OptTs] of int: optt_dur;

array [Jobs] of int: last_task = [ max(tasks[j]) | j in Jobs ];

%-----------------------------------------------------------------------------%
% Additional derived parameters for tasks

    % "Short cut" from a task to its job
    %
array [Tasks] of int: task_job = 
    [ min(j in Jobs where t in tasks[j])(j) | t in Tasks ];

    % Earliest start times of tasks
    %
array [Tasks] of int: task_mins =
    [ sum(k in tasks[task_job[t]])(if k < t then task_mind[k] else 0 endif)
    |   t in Tasks ];

    % Latest start times of tasks
    %
array [Tasks] of int: task_maxs =
    [ t_max -
        sum(k in tasks[task_job[t]])(if k < t then 0 else task_mind[k] endif)
    |   t in Tasks ];

    % Minimal durations of tasks
    %
array [Tasks] of int: task_mind = 
    [ min(o in optts[t])(optt_dur[o]) | t in Tasks ];

    % Maximal durations of tasks
    %
array [Tasks] of int: task_maxd = 
    [ max(o in optts[t])(optt_dur[o]) | t in Tasks ];

    % Additional deirved parameters for optional tasks
    %
array [OptTs] of int: optt_task =
    [ min(t in Tasks where o in optts[t])(t) | o in OptTs ];

int: min_dur = min(optt_dur);
int: max_dur = max(optt_dur);
set of int: Durs = min_dur..max_dur;

    % Parameters related to the planning horizon
    %
int: t_max = sum(t in Tasks)(max(o in optts[t])(optt_dur[o]));
set of int: Times = 0..t_max;

%-----------------------------------------------------------------------------%
% Variables

    % Start time variables for tasks
    %
array [Tasks] of var Times: start =
    [ let { var task_mins[t]..task_maxs[t]: s } in s | t in Tasks ];

    % Duration variables for tasks
    %
array [Tasks] of var Durs: dur =
    [ if task_mind[t] = task_maxd[t] then task_mind[t] else
        let { var task_mind[t]..task_maxd[t]: d } in d endif
    |   t in Tasks ];

    % Variables whether an optional task is executed
    %
array [OptTs] of var opt Times: optt_s = 
    [ let { var opt task_mins[optt_task[o]]..task_maxs[optt_task[o]]: s } in s | o in OptTs ];

var Times: objective ::output_var;

%-----------------------------------------------------------------------------%
% Constraints

    % Precedence relations
    %
constraint
    forall(j in Jobs, i in tasks[j] where i < last_task[j])(
        start[i] + dur[i] <= start[i + 1]
    );

    % Optional tasks' constraints
    %
constraint
    forall(t in Tasks)(
        alternative(start[t], dur[t], [ optt_s[o] | o in optts[t] ], [ optt_dur[o] | o in optts[t] ])
    );

predicate alternative(var int: s, var int: d, array[int] of var opt int: os, array[int] of int: od) =
  let {
    var min(index_set(os))..max(index_set(os)): m;
  } in (
    [occurs(os[i]) | i in index_set(os)][m] /\
    od[m] = d /\
    [deopt(os[i]) | i in index_set(os)][m] = s /\
    sum (i in index_set(os)) (bool2int(occurs(os[i]))) = 1
  );

    % Resource constraints
    %
constraint
    forall(m in Mach)(
        let {
            set of int: MTasks = { o | o in OptTs where optt_mach[o] = m }
        } in (
            disjunctive([optt_s[o] | o in MTasks], [optt_dur[o] | o in MTasks])
        )
    );

predicate disjunctive(array[int] of var opt int: s, array[int] of int: d) =
  forall(i, j in index_set(s) where i<j) (
    ( (s[i] `opt_plus` d[i]) `opt_le` s[j] \/
      (s[j] `opt_plus` d[j]) `opt_le` s[i] )
  );

constraint
    forall(j in Jobs)(start[last_task[j]] + dur[last_task[j]] <= objective);

%------------------------------------------------------------------------------%
% Solve item

solve 
    :: search
    minimize objective;

%------------------------------------------------------------------------------%
% Searches

ann: s_mindur   = int_search(dur, smallest, indomain_min, complete);
ann: s_minstart = int_search(start, smallest, indomain_min, complete);
ann: s_bool     = bool_search([ occurs(optt_s[o]) | o in OptTs], input_order, 
                    indomain_max, complete);
ann: s_obj      = int_search([objective], input_order, indomain_min, complete);

ann: search = seq_search([s_mindur, s_bool, s_minstart, s_obj]);

%-----------------------------------------------------------------------------%
% Output

output 
[   "objective = ", show(objective), ";\n",
    "start = ", show(start), ";\n",
    "dur = ", show(dur), ";\n"
];


function var opt int: opt_plus(var opt int:x, var opt int: y) =
  let { var opt lb(x)+lb(y)..ub(x)+ub(y): z;
  constraint occurs(x) /\ occurs(y) <-> occurs(z);
  constraint occurs(z) -> deopt(z)=deopt(x)+deopt(y);
  } in z;

function var opt int : maximum(array[int] of var opt int : x) =
  let { var opt lb_array(x)..ub_array(x) : y;
        constraint (occurs(y) <- exists(i in index_set(x))(occurs(x[i])))
                   /\ occurs(y) -> (exists(i in index_set(x))(occurs(x[i]) /\ x[i] `opt_eq` y)
                                   /\ forall(i in index_set(x))(x[i] `opt_le` y)) }
  in y;

function var bool: opt_le(var opt int : x, var opt int : y) =
  (occurs(x) /\ occurs(y)) -> deopt(x) <= deopt(y);

function var bool: opt_eq(var opt int : x, var opt int : y) =
  occurs(x) -> (occurs(y) /\ deopt(x) = deopt(y));

% tasks' durations on different machines are NOT the same
% all jobs have the same number of tasks
% Average number of machines per task 2.20
no_mach = 6;
no_jobs = 5;
no_task = 15;
no_optt = 33;
tasks = [1..3, 4..6, 7..9, 10..12, 13..15];
optts = [1..3, 4..5, 6..7, 8..9, 10..11, 12..13, 14..15, 16..17, 18..20, 21..22, {23}, 24..25, 26..28, 29..31, 32..33];
optt_mach = [1, 2, 3, 2, 4, 4, 5, 1, 3, 2, 3, 5, 6, 1, 2, 3, 4, 4, 5, 6, 1, 2, 5, 4, 6, 1, 2, 3, 3, 4, 5, 5, 6];
optt_dur = [147, 123, 145, 130, 140, 150, 160, 214, 150, 66, 87, 178, 95, 87, 62, 180, 105, 190, 100, 153, 87, 65, 173, 145, 136, 128, 123, 145, 86, 65, 47, 110, 85];

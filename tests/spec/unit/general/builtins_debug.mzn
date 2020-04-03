/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    assert_par_int: 1
    assert_var_int: 2
    assert_par_opt_int: 3
    assert_var_opt_int: 4
    assert_array_of_par_int: [1]
    assert_array_of_var_int: [2]
    assert_array_of_par_opt_int: [3]
    assert_array_of_var_opt_int: [4]
    trace_par_int: 1
    trace_var_int: 2
    trace_par_opt_int: 3
    trace_var_opt_int: 4
    trace_array_of_par_int: [1]
    trace_array_of_var_int: [2]
    trace_array_of_par_opt_int: [3]
    trace_array_of_var_opt_int: [4]
    logstream: "trace_logstream_par_int\ntrace_logstream_var_int\ntrace_logstream_par_opt_int\ntrace_logstream_var_opt_int\ntrace_logstream_array_of_par_int\ntrace_logstream_array_of_var_int\ntrace_logstream_array_of_par_opt_int\ntrace_logstream_array_of_var_opt_int\n"
***/

% Previously this would fail type-checking due to missing definitions of trace and assert

int: par_int = 1;
var 2..2: var_int;
opt int: par_opt_int = 3;
var opt 4..4: var_opt_int;

array[int] of int: array_of_par_int = [1];
array[1..1] of var 2..2: array_of_var_int;
array[int] of opt int: array_of_par_opt_int = [3];
array[1..1] of var opt 4..4: array_of_var_opt_int;

constraint not absent(var_opt_int) /\ not absent(array_of_var_opt_int[1]);

int: assert_par_int :: add_to_output = assert(true, "assert_par_int\n", par_int);
var 2..2: assert_var_int :: add_to_output = assert(true, "assert_var_int\n", var_int);
opt int: assert_par_opt_int :: add_to_output = assert(true, "assert_par_opt_int\n", par_opt_int);
var opt 4..4: assert_var_opt_int :: add_to_output = assert(true, "assert_var_opt_int\n", var_opt_int);

array[int] of int: assert_array_of_par_int :: add_to_output = assert(true, "assert_array_of_par_int\n", array_of_par_int);
array[1..1] of var 2..2: assert_array_of_var_int :: add_to_output = assert(true, "assert_array_of_var_int\n", array_of_var_int);
array[int] of opt int: assert_array_of_par_opt_int :: add_to_output = assert(true, "assert_array_of_par_opt_int\n", array_of_par_opt_int);
array[1..1] of var opt 4..4: assert_array_of_var_opt_int :: add_to_output = assert(true, "assert_array_of_var_opt_int\n", array_of_var_opt_int);

int: trace_par_int :: add_to_output = trace("trace_par_int\n", par_int);
var 2..2: trace_var_int :: add_to_output = trace("trace_var_int\n", var_int);
opt int: trace_par_opt_int :: add_to_output = trace("trace_par_opt_int\n", par_opt_int);
var opt 4..4: trace_var_opt_int :: add_to_output = trace("trace_var_opt_int\n", var_opt_int);

array[int] of int: trace_array_of_par_int :: add_to_output = trace("trace_array_of_par_int\n", array_of_par_int);
array[1..1] of var 2..2: trace_array_of_var_int :: add_to_output = trace("trace_array_of_var_int\n", array_of_var_int);
array[int] of opt int: trace_array_of_par_opt_int :: add_to_output = trace("trace_array_of_par_opt_int\n", array_of_par_opt_int);
array[1..1] of var opt 4..4: trace_array_of_var_opt_int :: add_to_output = trace("trace_array_of_var_opt_int\n", array_of_var_opt_int);

int: trace_stdout_par_int :: add_to_output = trace_stdout("% trace_stdout_par_int\n", par_int);
var 2..2: trace_stdout_var_int :: add_to_output = trace_stdout("% trace_stdout_var_int\n", var_int);
opt int: trace_stdout_par_opt_int :: add_to_output = trace_stdout("% trace_stdout_par_opt_int\n", par_opt_int);
var opt 4..4: trace_stdout_var_opt_int :: add_to_output = trace_stdout("% trace_stdout_var_opt_int\n", var_opt_int);

array[int] of int: trace_stdout_array_of_par_int :: add_to_output = trace_stdout("% trace_stdout_array_of_par_int\n", array_of_par_int);
array[1..1] of var 2..2: trace_stdout_array_of_var_int :: add_to_output = trace_stdout("% trace_stdout_array_of_var_int\n", array_of_var_int);
array[int] of opt int: trace_stdout_array_of_par_opt_int :: add_to_output = trace_stdout("% trace_stdout_array_of_par_opt_int\n", array_of_par_opt_int);
array[1..1] of var opt 4..4: trace_stdout_array_of_var_opt_int :: add_to_output = trace_stdout("% trace_stdout_array_of_var_opt_int\n", array_of_var_opt_int);

int: trace_logstream_par_int :: add_to_output = trace_logstream("trace_logstream_par_int\n", par_int);
var 2..2: trace_logstream_var_int :: add_to_output = trace_logstream("trace_logstream_var_int\n", var_int);
opt int: trace_logstream_par_opt_int :: add_to_output = trace_logstream("trace_logstream_par_opt_int\n", par_opt_int);
var opt 4..4: trace_logstream_var_opt_int :: add_to_output = trace_logstream("trace_logstream_var_opt_int\n", var_opt_int);

array[int] of int: trace_logstream_array_of_par_int :: add_to_output = trace_logstream("trace_logstream_array_of_par_int\n", array_of_par_int);
array[1..1] of var 2..2: trace_logstream_array_of_var_int :: add_to_output = trace_logstream("trace_logstream_array_of_var_int\n", array_of_var_int);
array[int] of opt int: trace_logstream_array_of_par_opt_int :: add_to_output = trace_logstream("trace_logstream_array_of_par_opt_int\n", array_of_par_opt_int);
array[1..1] of var opt 4..4: trace_logstream_array_of_var_opt_int :: add_to_output = trace_logstream("trace_logstream_array_of_var_opt_int\n", array_of_var_opt_int);

string: logstream :: add_to_output = logstream_to_string();
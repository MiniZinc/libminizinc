%-----------------------------------------------------------------------------%
% Requires that a set of tasks given by start times 's', durations 'd', and
% resource requirements 'r', never require more than a global resource bound
% 'b' at any one time.
% Assumptions:
% - forall i, d[i] >= 0 and r[i] >= 0
%-----------------------------------------------------------------------------%

predicate cumulative(array[int] of var int: s,
                     array[int] of var int: d,
                     array[int] of var int: r, var int: b) =
    assert(index_set(s) == index_set(d) /\ index_set(s) == index_set(r),
        "cumulative: the 3 array arguments must have identical index sets",
        assert(lb_array(d) >= 0 /\ lb_array(r) >= 0,
            "cumulative: durations and resource usages must be non-negative",
	g12fd_cumulative(s, d, r, b)));


%-----------------------------------------------------------------------------%

% Optional parameters that can be used with the built-in G12/FD cumulative
% constraint.
%
annotation energy_feasibility_check;
annotation edge_finding_filtering;
annotation ext_edge_finding_filtering;
annotation histogram_filtering;
annotation idempotent;

%-----------------------------------------------------------------------------%

% The implementation of the cumulative constraint in the G12/FD solver.
% This should not be called directly, instead the definition above should
% be used.
predicate g12fd_cumulative(array[int] of var int: s,
		     array[int] of var int: d,
		     array[int] of var int: r, var int: b);

%-----------------------------------------------------------------------------%

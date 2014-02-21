%-----------------------------------------------------------------------------%
% This file tests the global constraints in globals.mzn that involve
% variables of type `var float'.
%-----------------------------------------------------------------------------%

include "globals.mzn";

% Variable naming scheme:  all variables for a constraint get a common
% prefix based on the constraint name, eg. "alldiff".

%-----------------------------------------------------------------------------%
% increasing
%-----------------------------------------------------------------------------%

	% test satisfaction
array[1..3] of var float: inc_avf :: is_output = [1.0, 2.0, _];

constraint increasing(inc_avf);

%-----------------------------------------------------------------------------%
% minimum, maximum
%-----------------------------------------------------------------------------%

var 0.0..20.0: min_vf1 :: is_output;
var 0.0..20.0: min_vf2 :: is_output;

var 0.0..20.0: max_vf1 :: is_output;
var 0.0..20.0: max_vf2 :: is_output;

% XXX: min/2 for floats not yet handled
constraint minimum(min_vf1, [5.5, 3.3, 8.8, 0.0,     4.4]);
constraint minimum(0.0,     [5.5, 3.3, 8.8, min_vf2, 4.4]);

% XXX: max/2 for floats not yet handled
constraint maximum(max_vf1, [5.5, 3.3, 8.8, 9.9,     4.4]);
constraint maximum(9.9,     [5.5, 3.3, 8.8, max_vf2, 4.4]);

%-----------------------------------------------------------------------------%
solve satisfy;

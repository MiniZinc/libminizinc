% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

% Check that arrays of annotation varibles (and assignments to such) do
% not get emitted in .ozn files.

array[1..3] of ann: foo;
solve satisfy;
output ["Ok\n"];

foo = [first_fail, domain, bounds];

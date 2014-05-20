% RUNS ON mzn20_fd
% blocksworld_instance_1.mzn
% vim: ft=zinc ts=4 sw=4 et tw=0

include "blocksworld.model";

n_blocks = 3;

int: a = 1;
int: b = 2;
int: c = 3;

% Initial state:
%
%  C
%  A
%  B
% ---
%              A  B      C
initial_loc = [b, Table, a];

% Goal state:
%
%  A
%  B
%  C
% ---
%            A  B  C
final_loc = [b, c, Table];

n_steps = 4;

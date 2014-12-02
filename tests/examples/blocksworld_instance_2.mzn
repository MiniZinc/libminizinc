% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% blocksworld_instance_2.mzn
% vim: ft=zinc ts=4 sw=4 et tw=0

include "blocksworld.model";

n_blocks = 5;

int: a = 1;
int: b = 2;
int: c = 3;
int: d = 4;
int: e = 5;

% Initial state:
%
%  E
%  D
%  C
%  B
%  A
% ---
%              A      B  C  D  E
initial_loc = [Table, a, b, c, d];

% Goal state:
%
%  A
%  B
%  C
%  D
%  E
% ---
%            A  B  C  D  E
final_loc = [b, c, d, e, Table];

n_steps = 6;


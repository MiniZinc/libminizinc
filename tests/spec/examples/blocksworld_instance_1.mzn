/***
!Test
expected:
- !Result
  solution: !Solution
    'on':
    - [2, -2, 1]
    - [2, -2, -3]
    - [-1, -2, -3]
    - [2, 3, -3]
***/

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

% RUNS ON mzn_mer_fd
% RUNS ON zinc_fdic_mznlib
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

% factory_planning_instance.mzn
% vim: ft=zinc ts=4 sw=4 et tw=0

include "factory_planning.model";

n_machines = 5;
n_products = 2;
n_attributes = 3;
n_steps = 5;

input_machines = {1};

output_machines = {5};

% Connections:
%
%  M1 --- M2 ----.
%  |      |      |
%  |----- M3 --- M5
%  |      |      |
%  '----- M4 ----'

connected =
[|  1, 1, 1, 1, 0,
 |  1, 1, 1, 0, 1,
 |  1, 1, 1, 1, 1,
 |  1, 0, 1, 1, 1,
 |  0, 1, 1, 1, 1
|];

can_add_attr =
[|  0, 0, 0
 |  1, 0, 0
 |  0, 1, 0
 |  0, 0, 1
 |  0, 0, 0
|];

prod_start_mach = [
    1,
    1
];

prod_attr_goal =
[|  1, 1, 0
 |  1, 0, 1
|];

solve satisfy;

output [
    "factory planning instance\n",
    "step | product | location | a1 a2 a3\n",
    "-----+---------+----------+---------\n",
    " 1   | 1       | ", show(step_prod_mach[1, 1]), "        |  ",
        show(step_prod_attr[1, 1, 1]), "  ",
        show(step_prod_attr[1, 1, 2]), "  ",
        show(step_prod_attr[1, 1, 3]), "\n",
    "     | 2       | ", show(step_prod_mach[1, 2]), "        |  ",
        show(step_prod_attr[1, 2, 1]), "  ",
        show(step_prod_attr[1, 2, 2]), "  ",
        show(step_prod_attr[1, 2, 3]), "\n",
    "-----+---------+----------+---------\n",
    " 2   | 1       | ", show(step_prod_mach[2, 1]), "        |  ",
        show(step_prod_attr[2, 1, 1]), "  ",
        show(step_prod_attr[2, 1, 2]), "  ",
        show(step_prod_attr[2, 1, 3]), "\n",
    "     | 2       | ", show(step_prod_mach[2, 2]), "        |  ",
        show(step_prod_attr[2, 2, 1]), "  ",
        show(step_prod_attr[2, 2, 2]), "  ",
        show(step_prod_attr[2, 2, 3]), "\n",
    "-----+---------+----------+---------\n",
    " 3   | 1       | ", show(step_prod_mach[3, 1]), "        |  ",
        show(step_prod_attr[3, 1, 1]), "  ",
        show(step_prod_attr[3, 1, 2]), "  ",
        show(step_prod_attr[3, 1, 3]), "\n",
    "     | 2       | ", show(step_prod_mach[3, 2]), "        |  ",
        show(step_prod_attr[3, 2, 1]), "  ",
        show(step_prod_attr[3, 2, 2]), "  ",
        show(step_prod_attr[3, 2, 3]), "\n",
    "-----+---------+----------+---------\n",
    " 4   | 1       | ", show(step_prod_mach[4, 1]), "        |  ",
        show(step_prod_attr[4, 1, 1]), "  ",
        show(step_prod_attr[4, 1, 2]), "  ",
        show(step_prod_attr[4, 1, 3]), "\n",
    "     | 2       | ", show(step_prod_mach[4, 2]), "        |  ",
        show(step_prod_attr[4, 2, 1]), "  ",
        show(step_prod_attr[4, 2, 2]), "  ",
        show(step_prod_attr[4, 2, 3]), "\n",
    "-----+---------+----------+---------\n",
    " 5   | 1       | ", show(step_prod_mach[5, 1]), "        |  ",
        show(step_prod_attr[5, 1, 1]), "  ",
        show(step_prod_attr[5, 1, 2]), "  ",
        show(step_prod_attr[5, 1, 3]), "\n",
    "     | 2       | ", show(step_prod_mach[5, 2]), "        |  ",
        show(step_prod_attr[5, 2, 1]), "  ",
        show(step_prod_attr[5, 2, 2]), "  ",
        show(step_prod_attr[5, 2, 3]), "\n",
    "-----+---------+----------+---------\n"
];

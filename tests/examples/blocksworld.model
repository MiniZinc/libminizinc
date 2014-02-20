% blocksworld_model.mzn
% vim: ft=zinc ts=4 sw=4 et tw=0
%
% A state-based blocks-world model.

include "globals.mzn";

    % Instance parameter: the number of steps to consider in the plan.
    %
int: n_steps;

set of int: steps = 1..n_steps;

    % The last step.
    %
int: end = n_steps;

    % Instance parameter: the number of blocks in the problem.
    %
int: n_blocks;

set of int: blocks = 1..n_blocks;

    % Block 0 denotes the table, which has enough space for all the blocks.
    %
int: Table = 0;

set of int: table_or_blocks = {Table} union blocks;

    % Instance parameter: the starting locations of blocks in the problem.
    %
array [blocks] of table_or_blocks: initial_loc;

    % Instance parameter: the finishing locations of blocks in the problem.
    %
array [blocks] of table_or_blocks: final_loc;

    % If a block has a negative location then it's on the table.
    % Block a has its own "free" spot on the table at location -a.
    %
set of int: locs = -n_blocks..n_blocks;

    % Decision variables: block locations at each step.
    %
array [steps, blocks] of var locs: on;

    % Constrain the starting state.
    %
constraint
    forall (b in blocks) (
        if initial_loc[b] = Table then
            on[1, b] = -b
        else
            on[1, b] = initial_loc[b]
        endif
    );

    % Constrain the goal state.
    %
constraint
    forall (b in blocks) (
        if final_loc[b] = Table then
            on[end, b] = -b
        else
            on[end, b] = final_loc[b]
        endif
    );

    % Ensure each block cannot be put in the wrong place on the table
    % (this simplifies the model and allows us to use alldifferent below)
    % or on itself.
    %
constraint
    forall (b in blocks, s in steps) (
        on[s, b] in {c | c in blocks where c != b} union {-b}
    );

    % No two blocks can occupy the same location at the same time.
    %
constraint
    forall (s in steps) (alldifferent (b in blocks) (on[s, b]));

    % A clear block is one that has no other block on top of it.
    % The table is always clear.
    %
predicate clear(steps: s, var locs: l) =
    l < 0  \/  forall (b in blocks) (on[s, b] != l);

    % If a block moves then it must (a) be clear and (b) its destination
    % must be clear.
    %
constraint
    forall (s in steps, b in blocks where s < n_steps) (
        on[s, b] != on[s + 1, b]
    ->
        (   clear(s, b)
        /\  clear(s, on[s + 1, b])
        )
    );

solve   :: int_search(
            [on[s, b] | s in steps, b in blocks],
            first_fail,
            indomain_split,
            complete
        )
        satisfy;

output
    [   "[Negative locations denote the table.]\n"
    ] ++
    [   if b = 1 then
            "Step " ++ show(s) ++ ":\n"
        else
            ""
        endif ++
        "  block " ++ show(b) ++ " on " ++ show(on[s, b]) ++ "\n"
    |   s in 1..n_steps, b in 1..n_blocks
    ];


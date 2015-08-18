% blocksworld_model.mzn
% vim: ft=zinc ts=4 sw=4 et tw=0
%
% A state-based blocks-world model.
% Extended with MiniSearch

include "globals.mzn";

    % Instance parameter: the number of steps to consider in the plan.
    %
int: n_steps; % minimal number of steps

set of int: steps = 1..n_steps;

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

predicate keepAlive__(array[int,int] of var int);
 
constraint keepAlive__(on);

constraint
    forall (b in blocks) (
        if initial_loc[b] = Table then
            on[1, b] = -b
        else
            on[1, b] = initial_loc[b]
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
predicate clear(int: s, var locs: l) =
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

include "combinators.mzn";

int: iterations = 5;

solve   :: int_search(
            [on[s, b] | s in steps, b in blocks],
            first_fail,
            indomain_split,
            complete
        )
%        satisfy;
search blocksworld(iterations, on) /\ 
       if hasSol() then skip() else print("No solution found.\n") endif;



function ann: blocksworld(int: iterations,  array[int,int] of var int: on2) = 
  % if we find a solution with the minimal number of steps
  if scope(post(goalState(n_steps,on2)) /\
           next() 
     ) then commit() /\ print("Solved in minimal number of steps.\n")
  % otherwise iteratively increase the number of steps
  else
    repeat (i in 1..iterations) (
       scope (
          let {
             int: nbSteps = i+n_steps;
             set of int: Range = n_steps+1..nbSteps;
             array[Range, blocks] of var locs: on_plus; % additional moves after minimal number of steps
          } in (
             post(blocksworldConstraints(on_plus,Range,nbSteps)) /\
             if next() then commit() /\ print("Solved in "++show(nbSteps)++" steps.\n") 
                            % print the solution
                            /\ repeat (s in 1..nbSteps) (
                                 repeat (b in 1..n_blocks) (
                                    if b = 1 then print("Step " ++ show(s) ++ ":\n") else skip
                                    endif /\
                                    print("  block "++show(b) ++ " on " ++
                                           if s <= n_steps then show(sol(on2[s, b])) 
                                           else show(sol(on_plus[s,b]))
                                           endif
                                          ++ "\n")
                                 )
                               )
                            /\ break
             else skip /\ print("No solution found for "++show(nbSteps)++" steps.\n") endif
          )
       )
    )
  endif
;


predicate blocksworldConstraints(array[int,blocks] of var int: on_p, set of int: STEPS, int: lastStep) = 
    % Constrain the goal state.
    %
   goalState(lastStep, on_p)

    % Ensure each block cannot be put in the wrong place on the table
    % (this simplifies the model and allows us to use alldifferent below)
    % or on itself.
    %
/\
    forall (b in blocks, s in STEPS) (
        on_p[s, b] in {c | c in blocks where c != b} union {-b}
    )

    % No two blocks can occupy the same location at the same time.
    %
/\
    forall (s in STEPS) (alldifferent (b in blocks) (on_p[s, b]))


    % If a block moves then it must (a) be clear and (b) its destination
    % must be clear.
    %
/\
    forall (s in STEPS, b in blocks where s < lastStep) (
        on_p[s, b] != on_p[s + 1, b]
    ->
        (   clear_p(s, b, on_p)
        /\  clear_p(s, on_p[s + 1, b], on_p)
        )
    )

% connection constraint between on an on_p
/\
    forall (b in blocks) (
        on[n_steps, b] != on_p[n_steps+1, b]
    ->
        (   clear(n_steps, b)
        /\  clear(n_steps, on_p[n_steps+1, b])
        )
    )
;

    % A clear block is one that has no other block on top of it.
    % The table is always clear.
    %
predicate clear_p(steps: s, var locs: l, array[int,int] of var int: on_p) =
    l < 0  \/  forall (b in blocks) (on_p[s, b] != l);


predicate goalState(int: lastStep, array[int,int] of var int: on_) =
    % Constrain the goal state.
    %
    forall (b in blocks) (
        if final_loc[b] = Table then
            on_[lastStep, b] = -b  
            /\ trace("on["++show(lastStep)++", "++show(b)++"] = -"++show(b)++";\n")
        else
            on_[lastStep, b] = final_loc[b] 
            /\ trace("on["++show(lastStep)++", "++show(b)++"] = "++show(final_loc[b])++";\n")
        endif
    );

% -----------------------------------------------
%  Instance (instance1)
% -----------------------------------------------

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

n_steps = 5; % 6 in original instance


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
    ] ++ ["on = "++show(on)++"\n"];


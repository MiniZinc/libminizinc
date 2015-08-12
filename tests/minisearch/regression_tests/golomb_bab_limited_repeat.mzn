% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
%------------------------------------------------------------------------%
% Golomb rulers
% prob006 in csplib
%------------------------------------------------------------------------%
% From csplib:
% A Golomb ruler may be defined as a set of m integers 0 = a_1 < a_2 <
% ... < a_m such that the m(m-1)/2 differences a_j - a_i, 1 <= i < j
% <= m are distinct. Such a ruler is said to contain m marks and is of
% length a_m. The objective is to find optimal (minimum length) or
% near optimal rulers.
%
% This is the "ternary constraints and an alldifferent" model
%------------------------------------------------------------------------%

include "globals.mzn";
include "combinators.mzn"; % include the search combinators lite

int: m = 8; % 13
int: n = m*m;

var 0..n: obj;

array[int] of var 0..n: differences =
    [ mark[j] - mark[i] | i in 1..m, j in i+1..m];

array[1..m] of var 0..n: mark;

constraint mark[1] = 0;

constraint obj = mark[m];

constraint forall ( i in 1..m-1 ) ( mark[i] < mark[i+1] );

constraint alldifferent(differences);

    % Symmetry breaking
constraint differences[1] < differences[(m*(m-1)) div 2];

int: iterations = 30;
int: ms = 10*1000; % milliseconds
% solve using minimizing branch & bound
solve search time_limit(ms, my_bab(obj,iterations));

function ann: my_bab(var int: obj, int: iterations) =
         repeat (i in 1..iterations) (
                 print("Iteration "++show(i)++"\n") /\
                 if next then  % get next solution
                    print /\ % print intermediate solution
                    commit() /\
                    post(obj < sol(obj)) % post constraint
                 else break endif
         );
   

output ["golomb ", show(obj), "\n", show(mark)];

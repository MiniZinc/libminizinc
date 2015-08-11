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

int: m = 7;
int: n = m*m;

var 0..n: last_mark;
var 0..n: second_last_mark;
array[int] of var 0..n: differences =
    [ mark[j] - mark[i] | i in 1..m, j in i+1..m];

array[1..m] of var 0..n: mark;

constraint mark[1] = 0;

constraint last_mark = mark[m];

constraint second_last_mark = mark[m-1];

constraint forall ( i in 1..m-1 ) ( mark[i] < mark[i+1] );

constraint alldifferent(differences);

    % Symmetry breaking
constraint differences[1] < differences[(m*(m-1)) div 2];

% solve using minimizing branch & bound
solve search
       repeat (i in 1..4) (   
               if next() then % get next solution
                  print /\  % print intermediate solution
                  post(last_mark + second_last_mark <= sol(last_mark) + sol(second_last_mark) - 1) % post constraint
               else break endif
       ) /\
       print("Final solution = "++show(last_mark)++"\n");

output ["golomb ", show(last_mark), "\n", show(mark), "\n", show(second_last_mark)];

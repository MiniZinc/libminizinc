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

int: m = 6;
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

% solve using minimizing branch & bound
solve :: combinator(
   scope( 
       or([
         repeat (i in 1..11) (
             and([
                 next, % get next solution
                 print, % print intermediate solution
                 post(obj < sol(obj)) % post constraint
             ])
         ),
         print() % testing print with ()
       ])
   )
) 
satisfy;

output ["golomb ", show(obj), "\n", show(mark)];

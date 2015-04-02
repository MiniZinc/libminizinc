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

int: m = 14;
int: n = m*m;

var 0..n: obj;

array[1..m] of var 0..n: mark;

array[int] of var 0..n: differences =
    [ mark[j] - mark[i] | i in 1..m, j in i+1..m];

constraint mark[1] = 0;

constraint obj = mark[m];

constraint forall ( i in 1..m-1 ) ( mark[i] < mark[i+1] );

constraint alldifferent(differences);

    % Symmetry breaking
constraint differences[1] < differences[(m*(m-1)) div 2];


predicate neighbourhoodCts1(array[int] of var int: x, 
                            float: destructionRate) = 
   if(hasSol()) then 
     let {
       int: n = ceil((1.0 - destructionRate)*length(x));
       array[1..n] of int: idx = [ uniform(1,length(x)-1) | i in 1..n ]; % length(x)-1: do not keep last mark
     } in 
       forall (i in idx) (
         x[i] = sol(x[i])
       )     
   else true endif;

float: d = 0.7;
int: iterations = 10;
int: timeout1 = 30000; % ms timeout for all LNS iterations
int: timeout2 = 10000; % ms timeout for BAB in one iteration
int: timeout3 = 5000; % ms timeout for NEXT in one BAB iteration

% solve using minimizing branch & bound
solve :: combinator(
  time_limit(timeout1,
    repeat (i in 1..iterations) (
      and([
        scope(
          and([ 
            post(neighbourhoodCts1(mark,d)),
            time_limit(timeout2,
                       or([bab_min(obj,timeout3),print()]) %next, % or bab
            )
          ]),
        ),
        post(obj < sol(obj))
      ])
    )
  )
) 
satisfy;

output ["golomb ", show(obj), "\n", show(mark), "\n"];

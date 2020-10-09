/***
!Test
solvers: [gecode, chuffed] # Too slow on cbc
expected:
- !Result
  solution: !Solution
    x: [0, 70, 75, 0, 79, 50, 0, 50, 46, 27, 52, 35, 59, 35, 35, 50, 27, 52, 46, 75, 50]
    y: [0, 70, 33, 50, 0, 0, 85, 29, 88, 93, 70, 65, 54, 50, 82, 54, 85, 63, 82, 29, 63]
***/

% TOO LONG mzn20_fd_linear
% TOO LONG mzn20_mip
%-----------------------------------------------------------------------------
% Packing squares into a rectangle
%
% Guido Tack, tack@gecode.org
% 2007-02-22
%
% Ported from the Gecode example
%
%-----------------------------------------------------------------------------

%-----------------------------------------------------------------------------
% Specification

int: pack_x;
int: pack_y;
int: n;
array[1..n] of int: pack_s;

% Instance

%pack_x = 4;
%pack_y = 4;
%n = 4;
%pack_s = [2,2,2,2];

pack_x = 112;
pack_y = 112;
n = 21;
pack_s = [50,42,37,35,33,29,27,25,24,19,18,17,16,15,11,9,8,7,6,4,2];

%-----------------------------------------------------------------------------
% Model

array[1..n] of var 0..pack_x-1: x;
array[1..n] of var 0..pack_y-1: y;

constraint
  forall (i in 1..n) (
    x[i] <= pack_x - pack_s[i] /\
    y[i] <= pack_y - pack_s[i]
  )
  /\
  forall (i in 1..n, j in i+1..n) (
    x[j] - x[i] >= pack_s[i] \/
    x[i] - x[j] >= pack_s[j] \/
    y[j] - y[i] >= pack_s[i] \/
    y[i] - y[j] >= pack_s[j]
  );

% Symmetry breaking

constraint
  forall (i in 1..n-1) (
    if pack_s[i]=pack_s[i+1] then
      x[i] <= x[i+1]
    else
      true
    endif
  );

% Capacity constraints

constraint
  forall (cx in 0..pack_x-1) (
    sum (i in 1..n) (pack_s[i]*bool2int(x[i] in cx-pack_s[i]+1..cx)) = pack_x
  )
  /\
  forall (cy in 0..pack_y-1) (
    sum (i in 1..n) (pack_s[i]*bool2int(y[i] in cy-pack_s[i]+1..cy)) = pack_y
  );

solve ::seq_search([int_search(x,smallest,indomain_min,complete),
                    int_search(y,smallest,indomain_min,complete)])
      satisfy;

output [
    "packing ",show(n)," squares into a ",show(pack_x),"x",show(pack_y),
    " rectangle:\n"] ++
    [ "square "++show(i)++
      ", size "++show(pack_s[i])++"x"++show(pack_s[i])++
      ", at ("++show(x[i])++", "++show(y[i])++")\n" | i in 1..n];

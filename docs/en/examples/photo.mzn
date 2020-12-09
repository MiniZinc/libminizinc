int: n;                          % number of people
set of int: PERSON = 1..n;       % set of people
enum GENDER = { M, F, O };       % set of genders
array[PERSON] of GENDER: g;      % the gender of each person
set of int: POSN = 1..n;         % set of positions

array[PERSON] of var POSN: pos;  % decisions: a position for each person

array[POSN] of var PERSON: who;  % view: a person for each position
include "inverse.mzn";
constraint inverse(pos,who);     % channel from decisions to view
constraint forall(i in 1..n-2)
                 (g[who[i]] != g[who[i+1]] \/ g[who[i+1]] != g[who[i+2]]);

solve minimize sum(i in 1..n-1)(abs(pos[i] - pos[i+1]));

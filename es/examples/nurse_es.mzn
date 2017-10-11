% Simplificación de la lista de enfermeras.
include "regular.mzn";

enum NURSE;
enum DAY;
int: req_day;
int: req_night;
int: min_night;

enum SHIFT = { d, n, o };
int: S = card(SHIFT);

int: Q = 6; int: q0 = 1; set of int: STATE = 1..Q;
array[STATE,SHIFT] of int: t =
     [| 2, 3, 1    % Estado 1
      | 4, 4, 1    % Estado 2
      | 4, 5, 1    % Estado 3
      | 6, 6, 1    % Estado 4
      | 6, 0, 1    % Estado 5
      | 0, 0, 1|]; % Estado 6

array[NURSE,DAY] of var SHIFT: roster;

constraint forall(j in DAY)(
             sum(i in NURSE)(roster[i,j] == d) == req_day /\
             sum(i in NURSE)(roster[i,j] == n) == req_night
           );
constraint forall(i in NURSE)(
             regular([roster[i,j] | j in DAY], Q, S, t, q0, STATE) /\
             sum(j in DAY)(roster[i,j] == n) >= min_night
           );

solve satisfy;

output [ show(roster[i,j]) ++ if j==card(DAY) then "\n" else " " endif
       | i in NURSE, j in DAY ];

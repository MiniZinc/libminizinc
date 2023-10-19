% Simple nurse rostering
include "regular.mzn";
enum NURSE;
enum DAY;
int: req_day;
int: req_night;
int: min_night;

enum SHIFT = { d, n, o };

enum STATE = { S1, S2, S3, S4, S5, S6 };

array[STATE,SHIFT] of opt STATE: t =
     [|      d:  n:  o:
      | S1:  S2, S3, S1
      | S2:  S4, S4, S1
      | S3:  S4, S5, S1
      | S4:  S6, S6, S1
      | S5:  S6, <>, S1
      | S6:  <>, <>, S1|];

array[NURSE,DAY] of var SHIFT: roster;

constraint forall(j in DAY)(
             sum(i in NURSE)(roster[i,j] == d) == req_day /\
             sum(i in NURSE)(roster[i,j] == n) == req_night
           );
constraint forall(i in NURSE)(
             regular([roster[i,j] | j in DAY], t, S1, STATE) /\
             sum(j in DAY)(roster[i,j] == n) >= min_night
           );

solve satisfy;

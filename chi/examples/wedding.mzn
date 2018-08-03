enum Guests = { bride, groom, bestman, bridesmaid, bob, carol, 
  ted, alice, ron, rona, ed, clara}; 
set of int: Seats = 1..12;
set of int: Hatreds = 1..5;
array[Hatreds] of Guests: h1 = [groom, carol, ed, bride, ted];
array[Hatreds] of Guests: h2 = [clara, bestman, ted, alice, ron];
set of Guests: Males = {groom, bestman, bob, ted, ron,ed};
set of Guests: Females = {bride,bridesmaid,carol,alice,rona,clara}; 

array[Guests] of var Seats: pos; % 客人的座位
array[Hatreds] of var Seats: p1;  % 互相憎恶的客人1的座位 
array[Hatreds] of var Seats: p2;  % 互相憎恶的客人2的座位 
array[Hatreds] of var 0..1: sameside;  % 互相憎恶的客人是否坐在同一边 
array[Hatreds] of var Seats: cost;  % 互相憎恶的客人的距离

include "alldifferent.mzn";
constraint alldifferent(pos);
constraint forall(g in Males)( pos[g] mod 2 == 1 );
constraint forall(g in Females)( pos[g] mod 2 == 0 );
constraint not (pos[ed] in {1,6,7,12});
constraint abs(pos[bride] - pos[groom]) <= 1 /\ 
           (pos[bride] <= 6 <-> pos[groom] <= 6);
constraint forall(h in Hatreds)(
               p1[h] = pos[h1[h]] /\
               p2[h] = pos[h2[h]] /\
               sameside[h] = bool2int(p1[h] <= 6 <-> p2[h] <= 6) /\
               cost[h] = sameside[h] * abs(p1[h] - p2[h]) +
                  (1 - sameside[h]) * (abs(13 - p1[h] - p2[h]) + 1) );

solve maximize sum(h in Hatreds)(cost[h]);

output [ show(g)++" " | s in Seats,g in Guests where fix(pos[g]) == s]
       ++ ["\n"]; 

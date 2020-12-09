int: wa;
int: nt;
int: sa;
int: q;
int: nsw;
int: v;
int: t;

output [
  if wa!=nt /\ wa!=sa /\ nt!=sa /\ nt!=q /\ sa!=q /\
     sa!=nsw /\ sa!=v /\ q!=nsw /\ nsw!=v
  then "CORRECT\n"
  else "INCORRECT\n"
  endif
];

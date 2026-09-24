opt set of int: x;
opt set of int: y;
output [
  if occurs(x) /\ x = {1, 3} /\ absent(y) /\ card(x) = 2 /\ (x union y) = {1, 3} then
    "CORRECT"
  else
    "INCORRECT"
  endif
];

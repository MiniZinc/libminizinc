var int: item1; 
var int: item2;
var int: item3;
var int: item4;

constraint item1 + item2 + item3 + item4 == 711;
constraint item1 * item2 * item3 * item4 == 711 * 100 * 100 * 100;
  
constraint         0 < item1 /\ item1 <= item2
           /\ item2 <= item3 /\ item3 <= item4;
  
solve satisfy;

output ["{", show(item1), ",", show(item2), ",", show(item3), ",",
        show(item4),"}\n"];

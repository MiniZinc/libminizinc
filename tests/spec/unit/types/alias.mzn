/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: 3
    y: [null, 1]
    tup: [2, true]
***/
type MyInt = int;
MyInt: x ::output = 3;

type MyVarInt = var 1..1;
type AliasToMyVarInt = MyVarInt;
array[1..2] of opt AliasToMyVarInt: y ::output;
constraint absent(y[1]) > absent(y[2]);

type MyTuple = tuple(1..2, bool);
var MyTuple: tup ::output;
constraint tup.2 <-> (tup.1 mod 2 = 0);
constraint tup.2;
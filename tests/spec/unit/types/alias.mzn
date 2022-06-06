/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: 3
    y: 1
    tup: [2, true]
***/
type MyInt = int;
MyInt: x ::output = 3;

type MyVarInt = var 1..1;
type AliasToMyVarInt = MyVarInt;
AliasToMyVarInt: y ::output;

type MyTuple = tuple(var 1..2, var bool);
MyTuple: tup ::output;
constraint tup.2 <-> (tup.1 mod 2 = 0);
constraint tup.2;
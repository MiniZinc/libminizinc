/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x1: 1
    x2: 5
    x3: 2
    x4: 4
    x5: null
    x6: null
    y1: A
    y2: C
    y3: A
    y4: C
    y6: null
    y7: null
    a1: [1, 2]
    a2: 1
    a3: 2
    a4: [null, 1, 2]
    a5: 1
    a6: 2
    a7: [null]
    a8: null
    a9: null
    b1: [A, C]
    b2: A
    b3: C
    b4: [null, A, C]
    b5: A
    b6: C
    b7: [null]
    b8: null
    b9: null
***/

int: x1 :: output = min([3, 1, 5]);
int: x2 :: output = max([3, 1, 5]);
int: x3 :: output = min([<>, 2, 4]);
int: x4 :: output = max([<>, 2, 4]);
opt int: x5 :: output = min_weak([<>]);
opt int: x6 :: output = max_weak([<>]);

enum Foo = {A, B, C};

Foo: y1 :: output = min([B, A, C]);
Foo: y2 :: output = max([B, A, C]);
Foo: y3 :: output = min([<>, A, C]);
Foo: y4 :: output = max([<>, A, C]);
array [int] of opt Foo: y5 = [<>];
opt Foo: y6 :: output = min_weak(y5);
opt Foo: y7 :: output = max_weak(y5);

array [1..2] of var 1..2: a1;
constraint a1[1] < a1[2];
var int: a2 :: output = min(a1);
var int: a3 :: output = max(a1);
array [1..3] of var opt 1..2: a4;
constraint absent(a4[1]);
constraint occurs(a4[2]);
constraint occurs(a4[3]);
constraint a4[2] < a4[3];
var int: a5 :: output = min(a4);
var int: a6 :: output = max(a4);
array [1..1] of var opt 1..2: a7;
constraint absent(a7[1]);
var opt int: a8 :: output = min_weak(a7);
var opt int: a9 :: output = max_weak(a7);

array [1..2] of var {A, C}: b1;
constraint b1[1] < b1[2];
var Foo: b2 :: output = min(b1);
var Foo: b3 :: output = max(b1);
array [1..3] of var opt {A, C}: b4;
constraint absent(b4[1]);
constraint occurs(b4[2]);
constraint occurs(b4[3]);
constraint b4[2] < b4[3];
var Foo: b5 :: output = min(b4);
var Foo: b6 :: output = max(b4);
array [1..1] of var opt {A, C}: b7;
constraint absent(b7[1]);
var opt Foo: b8 :: output = min_weak(b7);
var opt Foo: b9 :: output = max_weak(b7);


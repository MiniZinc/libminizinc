/***
!Test
extra_files:
- json_enum_def.json
solvers: [gecode]
expected: !Result
  solution: !Solution
    w: A
    x: D
    y: G
    z: J
***/

enum P;
enum Q;
enum R;
enum S;

var P: w;
var Q: x;
var R: y;
var S: z;

solve minimize w + x + y + z;

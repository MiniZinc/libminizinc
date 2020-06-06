/***
!Test
expected:
- !Result
  solution: !Solution
    if5_5 : 1
    if5f_5 : 0
    if5_4 : 0
    if5f_5f : 1
    if2_set2_4 : 0
    ifTwoVars : 0
    ifTwoOptVars : 0
    ifEqualInitVars : 1
    ifEqualChainInitVars : 1
    ifOneVar : 1
    ifOneOptVar : 1
solvers:
- gecode
- cbc
***/

%-----------------------------------------------------------------------------%
var int: if5_5;
if5_5 == is_same(5, 5);

var int: if5f_5;
if5f_5 == is_same(5.0, 5);

var int: if5_4;
if5_4 == is_same(5, 4);

var int: if5f_5f;
if5f_5f == is_same(5.0, 5.0);

var int: if2_set2_4;
if2_set2_4 == is_same(2, {2, 4});

var int: ifTwoVars;
var int: var1;
var 0..15: var2;
ifTwoVars == is_same(var1, var2);

var int: ifTwoOptVars;
var opt int: varO1;
var opt 0..15: varO2;
ifTwoOptVars == is_same(varO1, varO2);

var int: ifEqualInitVars;
var int: varInit1 = 3;
var 0..15: varInit2 = 3;
ifEqualInitVars == is_same(varInit1, varInit2);

var int: ifEqualChainInitVars;
var opt int: varInterm = varO1;
var opt int: varChInit1 = varInterm;
var opt 0..15: varChInit2 = varO1;
ifEqualChainInitVars == is_same(varChInit1, varChInit2);

var int: ifOneVar;
var int: v;
ifOneVar == is_same(v, v);

var int: ifOneOptVar;
var opt -1.0..-0.5: varOF;
ifOneOptVar == is_same(varOF, varOF);

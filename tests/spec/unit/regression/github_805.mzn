/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

var -7..-1: IV264;
var bool: BV831;
var -8..8: IV263;

var -14..2: IV21199;
var -7..1: IV0;
var -7..1: IV1;
var -7..1: IV2;
var -7..1: IV3;
var -7..1: IV4;
var -7..1: IV5;
var -7..1: IV21205;

var -7..1: IV6;
var -7..1: IV7;
var -7..1: IV8;
var -7..1: IV9;
var -7..1: IV10;
var -7..1: IV11;

array [int] of var int: A1 = [IV264, BV831];

constraint IV21199 != IV9 \/ IV21199 = IV10;
constraint IV21199 = IV11;
constraint IV21199 = IV21205;

constraint A1[IV263] = IV0;
constraint A1[IV263] = IV1;
constraint A1[IV263] = IV2;
constraint A1[IV263] = IV3;
constraint A1[IV263] = IV4;
constraint A1[IV263] = IV5;
constraint A1[IV263] = IV6;
constraint A1[IV263] = IV7;
constraint A1[IV263] = IV8;
constraint A1[IV263] = IV9;
constraint A1[IV263] = IV10;
constraint A1[IV263] = IV11;

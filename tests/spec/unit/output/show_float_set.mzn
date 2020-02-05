/***
!Test
expected: !Result
  solution: !Solution:
    y:
    - '{}'
    - '4.0..infinity'
    - '{2.0,3.0}'
    - '2.0..2.0 union 3.0..3.0 union 4.0..infinity'
    - '-1.0..1.0'
    - '-1.0..1.0 union 4.0..infinity'
    - '-1.0..1.0 union 2.0..2.0 union 3.0..3.0'
    - '-1.0..1.0 union 2.0..2.0 union 3.0..3.0 union 4.0..infinity'
    - '{-3.0,-2.0}'
    - '-3.0..-3.0 union -2.0..-2.0 union 4.0..infinity'
    - '{-3.0,-2.0,2.0,3.0}'
    - '-3.0..-3.0 union -2.0..-2.0 union 2.0..2.0 union 3.0..3.0 union 4.0..infinity'
    - '-3.0..-3.0 union -2.0..-2.0 union -1.0..1.0'
    - '-3.0..-3.0 union -2.0..-2.0 union -1.0..1.0 union 4.0..infinity'
    - '-3.0..-3.0 union -2.0..-2.0 union -1.0..1.0 union 2.0..2.0 union 3.0..3.0'
    - '-3.0..-3.0 union -2.0..-2.0 union -1.0..1.0 union 2.0..2.0 union 3.0..3.0 union 4.0..infinity'
    - '-infinity..-4.0'
    - '-infinity..-4.0 union 4.0..infinity'
    - '-infinity..-4.0 union 2.0..2.0 union 3.0..3.0'
    - '-infinity..-4.0 union 2.0..2.0 union 3.0..3.0 union 4.0..infinity'
    - '-infinity..-4.0 union -1.0..1.0'
    - '-infinity..-4.0 union -1.0..1.0 union 4.0..infinity'
    - '-infinity..-4.0 union -1.0..1.0 union 2.0..2.0 union 3.0..3.0'
    - '-infinity..-4.0 union -1.0..1.0 union 2.0..2.0 union 3.0..3.0 union 4.0..infinity'
    - '-infinity..-4.0 union -3.0..-3.0 union -2.0..-2.0'
    - '-infinity..-4.0 union -3.0..-3.0 union -2.0..-2.0 union 4.0..infinity'
    - '-infinity..-4.0 union -3.0..-3.0 union -2.0..-2.0 union 2.0..2.0 union 3.0..3.0'
    - '-infinity..-4.0 union -3.0..-3.0 union -2.0..-2.0 union 2.0..2.0 union 3.0..3.0 union 4.0..infinity'
    - '-infinity..-4.0 union -3.0..-3.0 union -2.0..-2.0 union -1.0..1.0'
    - '-infinity..-4.0 union -3.0..-3.0 union -2.0..-2.0 union -1.0..1.0 union 4.0..infinity'
    - '-infinity..-4.0 union -3.0..-3.0 union -2.0..-2.0 union -1.0..1.0 union 2.0..2.0 union 3.0..3.0'
    - '-infinity..-4.0 union -3.0..-3.0 union -2.0..-2.0 union -1.0..1.0 union 2.0..2.0 union 3.0..3.0 union 4.0..infinity'
***/

array [1..5] of set of float: x = [
  -infinity..-4.0, 
  {-3.0, -2.0}, 
  -1.0..1.0, 
  {2.0, 3.0}, 
  4.0..infinity
];

function set of float: s(bool: b, set of float: x) = if b then x else {} endif;

array [1..32] of string: y :: add_to_output = [
  show(
    s(a, x[1]) union 
    s(b, x[2]) union
    s(c, x[3]) union
    s(d, x[4]) union
    s(e, x[5])
  ) | a, b, c, d, e in [false, true]
];
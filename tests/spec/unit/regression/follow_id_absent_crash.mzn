/***
!Test
expected: !Result
  solution: !Solution
    y: null
***/

% Following the id chain y -> x -> <> should stop at the declaration before <>
% Previously it would return <> and cause crashes during flattening

var opt bool: y :: add_to_output = x;
var opt bool: x = <>;


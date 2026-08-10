/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

% Unicode stops at U+10FFFF. Without the bound the UTF-8 encoder truncates and
% writes bytes that are not valid UTF-8.
string: s = "\U00110000";

solve satisfy;

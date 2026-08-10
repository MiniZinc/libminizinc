/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

% `enum E = ["a", "b"]` is a whole enum body rather than one case of a `++`
% chain (parser.yxx:567), and carries item annotations like the other forms.
% The type checker rejects this initialisation, but it has to *parse* first: if
% the grammar did not accept it the error would be a SyntaxError instead.
annotation foo;
enum E :: foo = ["a", "b"];

solve satisfy;

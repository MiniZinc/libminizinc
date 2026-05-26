/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
  regex: 'illegal expression in generator call'
type: compile
***/

% Named-argument syntax inside a generator-call head must be rejected. The
% comp_or_expr grammar is shared between call args and generator-call heads;
% the post-reduce handler in lib/parser.yxx already rejects any element that
% is neither an Id nor a BinOp, which catches the VarDecl sentinel produced
% by the named-args productions (parser.yxx:2387).

predicate p(int: i);
constraint forall(i: 1..3)(p(i));

solve satisfy;

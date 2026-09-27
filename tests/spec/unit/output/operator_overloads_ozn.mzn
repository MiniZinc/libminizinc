/***
--- !Test
type: output-model
solvers: [gecode]
expected: !OutputModel operator_overloads_ozn.ozn
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: "true 4 -11 {1.5,2.5}\n"
***/

% User-defined operators used in the output must be copied into the output model (.ozn), like
% functions. A specialised (polymorphic) operator is copied as a monomorphic overload of the
% operator, so that the operator resolves to it when the .ozn is parsed again.
function int: '+'(bool: a, string: s) = bool2int(a) + string_length(s);
function int: '-'(bool: a) = -bool2int(a) - 10;
function set of $T: 'union'(opt set of $T: x, set of $T: y) = (x default {}) union y;
var 2..3: x;
opt set of float: p = {1.5};
output ["\(fix(x) > 1) \((fix(x) > 1) + "abc") \(-(fix(x) > 1)) \(p union {2.5})\n"];

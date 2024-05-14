/***
--- !Test
type: compile
solvers: [fznjson.msc]
expected: !FlatZincJSON fznjson-basic.json
***/

var 1..3: x;
var 1..3: y;
var bool: b = x < y;
constraint b -> x + y < 4;

solve :: seq_search([
    int_search([x, y], input_order, indomain_min),
    bool_search([b], input_order, indomain_min),
]) satisfy;

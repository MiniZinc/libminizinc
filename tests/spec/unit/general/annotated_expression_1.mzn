annotation foo_ann(int: a, int: b, var int: x :: annotated_expression) = let {
    constraint a * x + b = 10;
} in empty_annotation;

var 1..10: x :: foo_ann(3, 1);

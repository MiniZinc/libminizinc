var 1..3: x;
var 1..3: y;

constraint trace_stdout("foo\n");

solve maximize x + y;

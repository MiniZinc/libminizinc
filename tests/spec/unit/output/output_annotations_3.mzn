/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    objective: 6
    x@15.3-23|i=1@14.23|sum@14.18-16.7|=@14.12-16.7: 1
    x@15.3-23|i=2@14.23|sum@14.18-16.7|=@14.12-16.7: 2
    x@15.3-23|i=3@14.23|sum@14.18-16.7|=@14.12-16.7: 3
***/

var int: obj :: no_output;

constraint obj = sum (i in 1..3) (let {
  var 1..i: x :: output;
} in x);

solve maximize obj;

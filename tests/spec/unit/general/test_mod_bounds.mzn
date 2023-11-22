/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    b: !Range 0..2
    c: !Range -2..2
    d: !Range -2..0
***/
var 1..10: x;
var -10..10: y;
var -10..-1: z;

var -2..3: a;

any: b :: output = compute_mod_bounds(x, a);
any: c :: output = compute_mod_bounds(y, a);
any: d :: output = compute_mod_bounds(z, a);

constraint forall (i in dom(x), j in dom(a) where j != 0) (assert(i mod j in b, "\(i) mod \(j) = \(i mod j) but bounds is \(b)"));
constraint forall (i in dom(y), j in dom(a) where j != 0) (assert(i mod j in c, "\(i) mod \(j) = \(i mod j) but bounds is \(c)"));
constraint forall (i in dom(z), j in dom(a) where j != 0) (assert(i mod j in d, "\(i) mod \(j) = \(i mod j) but bounds is \(d)"));

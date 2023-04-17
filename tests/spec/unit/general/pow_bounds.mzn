/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

% Test for computation of pow bounds
constraint forall (i, j, k, l in -3..3 where i <= j /\ k <= l) (
    let {
        var i..j: x;
        var k..l: y;
        set of int: b = compute_pow_bounds(x, y);
        set of int: r = { m ^ n | m in i..j, n in k..l where m != 0 \/ n >= 0 };
        set of int: real = if r = {} then {} else min(r)..max(r) endif;
    } in assert(b = real, "Expected computed bounds for base \(i..j), exponent \(k..l) to be \(real), but got \(b)")
);

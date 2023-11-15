/***
!Test
expected: !Result
  solution: !Solution
    xc: 3
    zc: 1
    bc: 1
***/

any: x = { 1.0, 2.0, 3.0};
any: xc ::output = card(x);

any: y = 1.0..2.0;
any: yc = card(y);
constraint assert(yc = infinity, "yc != infinity");

any: z = 1.0..1.0;
any: zc ::output = card(z);

any: a = 1.0..2.0 union { 3.0, 4.0 };
any: ac = card(a);
constraint assert(ac = infinity, "ac != infinity");

any: b = {false};
any: bc ::output = card(b);

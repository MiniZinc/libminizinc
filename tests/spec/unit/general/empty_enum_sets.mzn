/***
!Test
expected: !Result
  solution: !Solution
    s1: !!set {}
    s2: !!set {}
    s3: !!set {}
    s4: !!set {}
    s5: !!set {}
    s6: !!set {}
    s7: !!set {}
    s8: !!set {}
***/

enum X = {A, B, C};

any: s1 ::output = ..<A;
any: s2 ::output = C<..;
any: s3 ::output = A..<A;
any: s4 ::output = C<..C;
any: s5 ::output = A<..<A;
any: s6 ::output = C<..<C;
any: s7 ::output = <..<A;
any: s8 ::output = C<..<;

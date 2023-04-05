/***
!Test
solvers: [gecode]
extra_files:
- json_unicode_escapes.json
expected: !Result
  solution: !Solution
    a: 'A'
    mu: 'μ'
    arrow: '↑'
    clef: '𝄞'
***/

string: a :: output;
string: mu :: output;
string: arrow :: output;
string: clef :: output;

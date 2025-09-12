/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

set of int: my_domain = 1..5;
type DomainAlias = 1..3;
type Alias = tuple(DomainAlias, my_domain);
set of float: another_domain = 1.5..2.5;
function Alias: foo(another_domain: x, tuple(another_domain): y) = (3, 5);
function set of DomainAlias: bar(my_domain: x) = 1..x;
bar(3): b = 2;

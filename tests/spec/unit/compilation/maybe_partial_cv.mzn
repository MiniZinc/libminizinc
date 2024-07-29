/***
!Test
solvers: [gecode]
options:
  -Werror: true
expected: !Result
  status: SATISFIED
***/

int: x = if has_element(1, col([||], 2))::maybe_partial then 1 else 0 endif;
int: y = if (col([||], 2)[1]=1) ::maybe_partial then 1 else 0 endif;

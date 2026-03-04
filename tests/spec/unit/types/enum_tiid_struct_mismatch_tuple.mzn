/***
!Test
solvers: [gecode]
type: compile
expected: !Error
  message: 'type-inst variable $T instantiated with incompatible types (tuple(CType) vs tuple(BType)) ($T.1 has enum mismatch: `BType` vs `CType`)'
  type: TypeError
***/

enum CType = {C_A};
enum BType = {B_A};

constraint (C_A,) = (B_A,);

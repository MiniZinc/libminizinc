/***
!Test
solvers: [gecode]
type: compile
expected: !Error
  message: 'type-inst variable $X instantiated with incompatible types (record(var CType: b_type, var CType: c_type) vs record(BType: b_type, CType: c_type)) ($X.b_type has enum mismatch: `BType` vs `CType`)'
  type: TypeError
***/

enum CType = {C_A};
enum BType = {B_A};

array[1..1] of var CType: C;
array[int] of record(CType: c_type, BType: b_type): Tab = [
    (c_type: C_A, b_type: B_A)
];

constraint (c_type: C[1], b_type: C[1]) in Tab;

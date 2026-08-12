/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: |-
      [(1, 2)]|[1]|[3]
      [(a: 1, b: 2)]|[1]
      [(1, 2)]|[(a: 1, b: 2)]
      [(1, 2), (3, 4)]|true
***/

% Regression test for projecting a struct-valued field out of an array of structs.
%
% A field access on an array is a projection, so its type is an array type. The type id of an
% array type must always be an array enum id, but the projection used to store the *element's*
% type id there whenever none of the index sets were enums. Downstream code that resolves the
% element type (`EnvI::getStructType' via `TypeInst::setStructDomain') then interpreted the
% struct type id as an array enum id and aborted with
%   "Assertion failed: (i > 0 && i <= _arrayEnumDecls.size()), function getArrayEnum".
% Only struct-valued fields are affected: a scalar field has type id 0, which is a valid
% (absent) array enum id. Whether it aborted, silently resolved the wrong struct type, or
% happened to give the right answer depended on how many array enums had been registered,
% so both the tuple and the record cases are pinned down here.

% Field is a tuple, object is an array of tuples (the originally reported case, including the
% chained access `x.1.1' which projects twice).
any: att = [((1, 2), 3)];
% Field is a record, object is an array of records.
any: arr = [(r: (a: 1, b: 2), s: 3)];
% Mixed nesting: tuple field of an array of records, record field of an array of tuples.
any: art = [(r: (1, 2), s: 3)];
any: atr = [((a: 1, b: 2), 3)];

% Enum index sets must survive the projection, since that is the case the old code did handle
% (it registered an array enum only when an index set was an enum).
enum E = {A, B};
array[E] of any: ate = [A: ((1, 2), 5), B: ((3, 4), 6)];

output [
  show(att.1), "|", show(att.1.1), "|", show(att.2), "\n",
  show(arr.r), "|", show(arr.r.a), "\n",
  show(art.r), "|", show(atr.1), "\n",
  show(ate.1), "|", show(index_set(ate.1) == E),
];

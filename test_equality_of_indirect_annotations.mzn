/***
!Test
strict: true
expected: !Result
  status: SATISFIED
***/

% Equality of annotation expression was failing if the annotation was an input argument (behind some indirection)
ann: some_ann;
ann: another_ann;
function bool: is_some_ann(ann: input_ann) =
  input_ann == some_ann;

constraint is_some_ann(some_ann);

% constraint trace("some_ann == some_ann \(some_ann == some_ann)\n"); % true :)
% constraint trace("some_ann == another_ann \(some_ann == another_ann)\n"); % false :)
% constraint trace("is_some_ann(some_ann) \(is_some_ann(some_ann))\n"); % was false :(
% constraint trace("is_some_ann(another_ann) \(is_some_ann(another_ann))\n"); % false :)

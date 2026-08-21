# Sets, for a solver that has none

The set constraints written over arrays of Booleans, one `fzn_<ident>.mzn` per
registry name. A set variable becomes the array saying which members it holds;
`fznso_set_to_bools.mzn` in `fznso_rewrite/` holds that conversion and the
reverse map that reads a solution back out.

This directory goes on the include path **only when the solver declares no set
decision variables**, and ahead of `fznso_constraints/`, so each file here
shadows the abort there. The decompositions are `std/nosets.mzn`'s own.

It is a directory rather than an include because `std/nosets.mzn` works by
redefining the set builtins, and `fznso_rewrite/redefinitions-fznso.mzn` already
does. Two libraries defining one builtin is a duplicate definition, not an
override, so the same decompositions had to move to where a name is defined
once — beside the constraint it belongs to.

`set_intersect`, `set_union`, `set_diff`, `set_symdiff` and element are
functions in `nosets.mzn`, and the registry names the result rather than
returning it. Those bodies are kept verbatim under private names in
`fznso_set_operations.mzn` and adapted, rather than reshaped: a reshaped
decomposition is a new one, and this is not the place to be writing them.

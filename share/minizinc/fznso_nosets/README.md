# Sets, for a solver that has none

The set constraints written over arrays of Booleans, one `fzn_<ident>.mzn` per
registry name. A set variable becomes the array saying which members it holds;
`fznso_set_to_bools.mzn` in `fznso_rewrite/` holds that conversion and the
reverse map that reads a solution back out.

This directory goes on the include path **only when the solver declares no set
decision variables**, and ahead of `fznso_constraints/`, so each file here
shadows the abort there. The decompositions are `std/nosets.mzn`'s own.

It is a directory rather than an include because `std/nosets.mzn` works by
redefining the set builtins, and `fznso_rewrite/redefinitions.mzn` already does.
Two libraries defining one builtin is a duplicate definition, not an override,
so the same decompositions had to move to where a name is defined once — beside
the constraint it belongs to.

## The two files `redefinitions.mzn` includes

Not everything `std/nosets.mzn` does hangs off a builtin, so two files here are
pulled in by the one file MiniZinc always reads. Each has an empty counterpart
in `fznso_constraints/`, so the include still resolves for a solver that does
have set variables.

`fznso_set_functions.mzn` — the four operations as functions returning a set.
The registry names the result instead, and `fzn_set_of_int_intersect` and its
siblings state that form, but MiniZinc introduces the result variable *before*
it flattens the operation and narrows its domain *after*. By then the result is
an array of Booleans and nothing re-applies the narrowing, so
`(s intersect t) == {}` holds with `s` and `t` sharing a member. A solver with
set variables is unaffected: the narrowing lands in a declaration it enforces
itself.

`fznso_set_search.mzn` — `set_search` and `warm_start`. An annotation keeps a
set variable's declaration alive after every constraint on it has been
decomposed, and it then reaches a solver that has just said it has none.

## Where these differ from `std/nosets.mzn`

Three places, each commented in the file:

- **`fzn_set_of_int_le_reif`** answers `b` where `std` answers `true` when
  `ub(s)` is empty. An empty `s` is below every `t`, so the reification is `b`;
  `std` leaves it free.
- **`fzn_int_in`** states its bounds as conjuncts rather than `let` constraints,
  and short-circuits a fixed contiguous set. Both matter for a solver that needs
  finite bounds and would otherwise pay for an element lookup per value.
- **The four set operations** state themselves over the result's own bits
  rather than as an equality against a returned set, for the ordering reason
  above. They are unreachable from MiniZinc, which takes the function form, but
  they are what a consumer calling the registry predicate gets.

## Not carried across

The `_imp` forms, on purpose: declaring `<ident>_imp` is what stops MiniZinc
falling back to `<ident>_reif`. See `fznso_constraints/README.md`.

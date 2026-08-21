# Rewriting the standard library onto the registry

Nothing here is a solver's concern. Every file exists to turn a name MiniZinc
uses into the name the FZnSO registry gives the same constraint; the constraints
themselves are in `fznso_constraints/`.

Two kinds of file:

- **At `std`'s filenames.** `fzn_all_different_int.mzn` keeps the predicate
  `std`'s `all_different` calls, and forwards it to `fzn_int_all_different`.
  The filename has to be `std`'s, because that is what its `include` resolves.
  These also reshape what a solver receives: flat one-based arrays and explicit
  index offsets, in place of the `index_set` a solver never sees.
- **`redefinitions*.mzn`.** The FlatZinc builtins. MiniZinc emits a builtin
  whatever a solver declares, so each one is rewritten onto its registry name —
  `array_bool_and` to `bool_array_and`, `set_eq` to `set_of_int_eq`, `bool2int`
  to `bool_to_int`. A solver behind this interface ships no `redefinitions*.mzn`
  of its own: two libraries defining one builtin is a duplicate definition
  rather than an override.

## Reified, but not half-reified

Each builtin is rewritten together with its reified form, so `set_eq_reif` lands
on `set_of_int_eq_reif` the same way `set_eq` lands on `set_of_int_eq`.

There is no `_imp` rewrite. `flatzinc_builtins.mzn` declares no `_imp` at all,
and declaring one here would stop MiniZinc falling back to the reified builtin —
which is the thing a solver is most likely to implement natively. See
`fznso_constraints/README.md`.

## The builtins that are not rewritten

The registry has no name for `int_eq`, `int_lt`, `int_plus`, `bool_and`,
`float_max` and about thirty others, so they reach a solver under their own
name. `redefinitions-fznso.mzn` lists them. Inventing an `fzn_` spelling would
put a name in the solver-facing layer that the specification does not define,
which is worse than the gap: the fix is a registry entry, not a rewrite.

## `nosets.mzn`

Shadowed here, and it redefines no builtin. Whether set variables get decomposed
is not a model's decision behind this interface — the solver says whether it has
them, and one that does not gets `fznso_nosets/` ahead of `fznso_constraints/`
on the include path. What the shadow keeps is the machinery a model may use
directly: `set2bools`, `setarray2bools`, `reverse_map_ab2si`.

## A note on overloading

`list of T` and `array [int] of T` are one type to the overload resolver, so the
two forms of a predicate can only coexist when they differ in arity, in the
dimension of some argument, or in the position and name of one. That is why
`fzn_cumulatives` takes its machine offset in the middle and calls its bound
array `capacity`.

A rewritten builtin must also keep the parameter *names*
`flatzinc_builtins.mzn` gives it. MiniZinc cannot tell apart two declarations of
one name that differ only there, and reports an internal error rather than a
type error when it tries.

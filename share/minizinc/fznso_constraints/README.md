# The FZnSO constraints

One `fzn_<ident>.mzn` per constraint the registry defines, and nothing else —
except the half-reified forms, which are deliberately absent. See below.
The file is named after the constraint rather than after whatever MiniZinc
happens to call it, so a solver writer looking for `int_all_different` opens
`fzn_int_all_different.mzn` and not `fzn_all_different_int.mzn`.

**This directory is the whole of what a solver concerns itself with.** The
translation from the standard library's own spelling lives next door in
`fznso_rewrite/`, which nothing here depends on.

## Overriding a constraint

Two ways, and a solver usually wants the first:

- **Declare the identifier.** A constraint a solver declares through
  `constraint_types` is native, so the declaration here is matched and its body
  is dropped. Nothing else is needed.
- **Ship a file of the same name** in the solver's own `mznlib`, which is
  searched first. That is for a solver that wants a decomposition of its own —
  because it has a propagator for a narrower case, or none at all and a better
  encoding than the one here.

## What the bodies are

Every file has a body, so a solver that declares none of this still gets a
working model — with two exceptions, both of which say so in the file:

- **A constraint a FlatZinc builtin backs** aborts. It is primitive: every way
  of writing what it means uses the operator it defines, so a body would flatten
  straight back to the same identifier and the compiler would not terminate.
  A solver either implements it or supplies its own file.
- **A constraint nothing reaches yet** aborts too. No MiniZinc global or builtin
  lowers to it, so there is no standard decomposition to carry across.

Everything else carries the standard library's own decomposition, which is why a
model that decomposes here decomposes to exactly what `fzn-<solver>` would have
given it.

## No `_imp` files

There is no `fzn_<ident>_imp.mzn` anywhere here, on purpose.

Declaring `<ident>_imp` is exactly what stops MiniZinc falling back to
`<ident>_reif`, so a library that declares one for every constraint takes the
solver's own reified propagator away from it — and gives back only what this
layer could write, which is the reified form again or an abort. Leaving the name
undeclared is strictly better: MiniZinc reifies fully, using whatever the solver
implements.

A solver that has a half-reified propagator, or wants a decomposition for one,
declares it beside its own `fzn_<ident>_reif.mzn`. That file is on the include
path wherever a reified form is needed, so it is the right place for both.

## `list of`, not `array [int] of`

`list of` is `array [1..infinity] of`: it states the one-basing a solver receives
anyway, and the type-checker holds the caller to it.

Where a constraint reads one array's values as indices into another —
`int_inverse`, `int_set_channel`, `int_bin_packing_load`, `int_array_range`,
`int_array_roots`, `int_cumulatives` and the rest — the index the array is
numbered from travels next to it as a plain `int`, because the flat array itself
cannot carry one.

A solver meets such an offset by padding or shifting its own array, which it can
only do from zero up, so the rewrite layer keeps the decomposition for a caller
numbering from below zero.

## Two notes

`fzn_int_set_channel.mzn` is the one file holding two overloads of one name: the
registry and the standard library spell that constraint the same, so there is
nothing to rewrite and both forms live here.

A few files carry a private helper beside the constraint — `fzn_cumulative_time`
in `fzn_int_cumulative.mzn`, `fzn_piecewise_linear_base`,
`strengthen_injection_for_inverse_in_range`. They are what the lifted body
calls, and only that body calls them.

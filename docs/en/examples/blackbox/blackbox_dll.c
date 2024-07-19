/*
 * Template: MiniZinc black-box propagator, dynamically loaded library (DLL) mode.
 *
 * Build (Linux):  cc -shared -fPIC -o libblackbox.so  blackbox_dll.c
 * Build (macOS):  cc -dynamiclib   -o libblackbox.dylib blackbox_dll.c
 * Build (Windows): cl /LD blackbox_dll.c
 *
 * Use from MiniZinc:
 *   predicate my_prop(array[int] of var int: xs)
 *       ::minizinc_value_propagator ::blackbox_dll("libblackbox.so");
 *
 * The solver loads the library once and resolves `fzn_blackbox`, which it calls
 * once per propagation. Per-use state is kept in an opaque *instance*: the
 * solver creates one per constraint with `fzn_init`, hands each parallel worker
 * its own copy via `fzn_clone`, and releases them with `fzn_free`. A library
 * provides exactly one propagator; use separate libraries for separate ones.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * The solver calls these entry points through a function pointer that uses the
 * platform default calling convention: `__stdcall` on Windows, the ordinary C
 * convention elsewhere. On Windows the symbols must also be exported explicitly
 * so that `GetProcAddress` can find them (otherwise `cl /LD` exports nothing).
 * Getting this wrong corrupts the stack on 32-bit Windows.
 */
#ifdef _WIN32
#define FZN_BLACKBOX_EXPORT __declspec(dllexport)
#define FZN_BLACKBOX_CALL __stdcall
#else
#define FZN_BLACKBOX_EXPORT
#define FZN_BLACKBOX_CALL
#endif

/*
 * Per-instance state. Keep everything that a propagation call depends on here,
 * not in globals, so that concurrent uses cannot interfere. A real library
 * would typically hold, e.g., a loaded/compiled program in this struct.
 */
typedef struct {
  int64_t parity; /* 0: accept even sums, 1: accept odd sums (from the args) */
} Instance;

/*
 * Optional: create the instance for one black-box constraint. `args` are the
 * strings from the second argument of the ::blackbox_dll annotation (an
 * argv-style list, possibly empty). Return an opaque pointer that is passed
 * back to every `fzn_blackbox` call. Omit this function for a stateless library
 * (the instance is then NULL).
 */
FZN_BLACKBOX_EXPORT void* FZN_BLACKBOX_CALL fzn_init(const char** args, size_t args_size) {
  Instance* inst = (Instance*)malloc(sizeof(Instance));
  inst->parity = 0;
  for (size_t i = 0; i < args_size; ++i) {
    if (strcmp(args[i], "odd") == 0) {
      inst->parity = 1;
    }
  }
  return inst;
}

/*
 * Optional: return an independent copy of an instance so that another worker
 * can use it concurrently. The copy must behave identically to its source.
 *
 * A thread-safe library that keeps no mutable per-call state can instead
 * reference-count the state here (bump a counter and `return instance;`) and
 * only free it in `fzn_free` once the count reaches zero, avoiding the copy.
 */
FZN_BLACKBOX_EXPORT void* FZN_BLACKBOX_CALL fzn_clone(void* instance) {
  Instance* copy = (Instance*)malloc(sizeof(Instance));
  *copy = *(const Instance*)instance;
  return copy;
}

/* Optional: release an instance produced by `fzn_init` or `fzn_clone`. */
FZN_BLACKBOX_EXPORT void FZN_BLACKBOX_CALL fzn_free(void* instance) { free(instance); }

/*
 * VALUE propagator:  `int_in` / `float_in` hold the assigned input values
 *                    (one entry per input variable). Write one value per
 *                    output variable to `int_out` / `float_out`.
 *
 * BOUNDS propagator: every array holds two entries per variable, a lower and
 *                    an upper bound: [lb0, ub0, lb1, ub1, ...]. Write the
 *                    tightened [lb, ub] pairs to the output arrays.
 *
 * `instance` is the pointer returned by `fzn_init` (NULL for a stateless
 * library). It is only ever used from one thread at a time.
 */
FZN_BLACKBOX_EXPORT void FZN_BLACKBOX_CALL fzn_blackbox(void* instance, const int64_t* int_in,
                                                        size_t int_in_size, const double* float_in,
                                                        size_t float_in_size, int64_t* int_out,
                                                        size_t int_out_size, double* float_out,
                                                        size_t float_out_size) {
  /* Example (relational value propagator): accept (1) iff the integer inputs
   * sum to the parity configured for this instance, reject (0) otherwise. */
  const Instance* inst = (const Instance*)instance;
  int64_t sum = 0;
  for (size_t i = 0; i < int_in_size; ++i) {
    sum += int_in[i];
  }
  if (int_out_size > 0) {
    int_out[0] = ((sum & 1) == inst->parity) ? 1 : 0;
  }

  (void)float_in;
  (void)float_in_size;
  (void)float_out;
  (void)float_out_size;
}

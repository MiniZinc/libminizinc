//! Template: MiniZinc black-box propagator, dynamically loaded library (DLL) mode.
//!
//! Cargo.toml:
//!   [lib]
//!   crate-type = ["cdylib"]
//! Build:  cargo build --release   (produces libblackbox.so / .dylib / .dll)
//!
//! Use from MiniZinc:
//!   predicate my_prop(array[int] of var int: xs)
//!       ::minizinc_value_propagator ::blackbox_dll("libblackbox.so");
//!
//! The solver loads the library once and resolves `fzn_blackbox`, calling it
//! once per propagation. Per-use state is kept in an opaque *instance*: the
//! solver creates one per constraint with `fzn_init`, hands each parallel worker
//! its own copy via `fzn_clone`, and releases them with `fzn_free`.

// The entry points use `extern "system"`, which is the platform default calling
// convention the solver calls through: `stdcall` on 32-bit Windows and the
// ordinary C convention everywhere else. `#[no_mangle]` on a `cdylib` exports
// the symbol so the solver can resolve it.
use std::ffi::CStr;
use std::os::raw::{c_char, c_double, c_void};

/// Per-instance state. Keep everything a propagation depends on here, not in
/// globals, so concurrent uses cannot interfere. A real library would hold,
/// e.g., a loaded/compiled program in this struct.
#[derive(Clone)]
struct Instance {
    parity: i64, // 0: accept even sums, 1: accept odd sums (from the args)
}

/// Optional: create the instance for one black-box constraint. `args` are the
/// strings from the second argument of the `::blackbox_dll` annotation (an
/// argv-style list, possibly empty). Returns an opaque pointer passed back to
/// every `fzn_blackbox` call. Omit for a stateless library (instance is null).
///
/// # Safety
/// `args` points to `args_size` valid, NUL-terminated C strings.
#[no_mangle]
pub unsafe extern "system" fn fzn_init(args: *const *const c_char, args_size: usize) -> *mut c_void {
    let mut inst = Instance { parity: 0 };
    for &p in std::slice::from_raw_parts(args, args_size) {
        if CStr::from_ptr(p).to_bytes() == b"odd" {
            inst.parity = 1;
        }
    }
    Box::into_raw(Box::new(inst)) as *mut c_void
}

/// Optional: return an independent copy of an instance for concurrent use by
/// another worker; the copy must behave identically to its source.
///
/// A thread-safe library that keeps no mutable per-call state can instead wrap
/// its state in an `Arc`, clone the `Arc` here, and avoid copying entirely.
///
/// # Safety
/// `instance` is a pointer previously returned by `fzn_init` / `fzn_clone`.
#[no_mangle]
pub unsafe extern "system" fn fzn_clone(instance: *mut c_void) -> *mut c_void {
    let inst = &*(instance as *const Instance);
    Box::into_raw(Box::new(inst.clone())) as *mut c_void
}

/// Optional: release an instance produced by `fzn_init` or `fzn_clone`.
///
/// # Safety
/// `instance` is a pointer previously returned by `fzn_init` / `fzn_clone`.
#[no_mangle]
pub unsafe extern "system" fn fzn_free(instance: *mut c_void) {
    drop(Box::from_raw(instance as *mut Instance));
}

/// VALUE propagator:  the input slices hold the assigned values (one per input
///                    variable); write one value per output variable.
///
/// BOUNDS propagator: every slice holds two entries per variable, a lower then
///                    an upper bound: `[lb0, ub0, lb1, ub1, ...]`. Write the
///                    tightened `[lb, ub]` pairs to the output slices.
///
/// `instance` is the pointer returned by `fzn_init` (null for a stateless
/// library); it is only ever used from one thread at a time.
///
/// # Safety
/// The pointers and sizes are provided by the solver and describe valid arrays.
#[no_mangle]
pub unsafe extern "system" fn fzn_blackbox(
    instance: *mut c_void,
    int_in: *const i64,
    int_in_size: usize,
    float_in: *const c_double,
    float_in_size: usize,
    int_out: *mut i64,
    int_out_size: usize,
    float_out: *mut c_double,
    float_out_size: usize,
) {
    let inst = &*(instance as *const Instance);
    let int_in = std::slice::from_raw_parts(int_in, int_in_size);
    let _float_in = std::slice::from_raw_parts(float_in, float_in_size);
    let int_out = std::slice::from_raw_parts_mut(int_out, int_out_size);
    let _float_out = std::slice::from_raw_parts_mut(float_out, float_out_size);

    // Example (relational value propagator): accept (1) iff the integer inputs
    // sum to the parity configured for this instance, reject (0) otherwise.
    if let Some(first) = int_out.first_mut() {
        *first = ((int_in.iter().sum::<i64>() & 1) == inst.parity) as i64;
    }
}

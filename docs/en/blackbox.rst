.. _ch-blackbox:

Black-Box Propagators (experimental)
====================================

.. warning::

  Black-box propagators are an **experimental** feature. The modelling
  interface, the external interfaces described here, and the set of supporting
  solvers may change in future releases.

A *black-box propagator* is a constraint whose propagation is performed by an
external implementation (e.g. an executable program or a dynamically loaded
library) rather than by decomposing the constraint into solver-primitive
constraints. This makes it possible to add problem-specific propagation without
modifying a solver, and to couple a model with external components such as
simulations or legacy code (hybrid white-box/black-box optimisation).

A supporting solver executes the black-box through a generic callback interface,
so the model remains solver-independent. Two kinds of propagator are supported:

* a **value propagator** is scheduled once all of its input variables are
  fixed, and either checks the assignment (a *relational* propagator) or
  computes output values (a *functional* propagator);
* a **bounds propagator** is scheduled whenever an input bound changes, and
  computes tightened bounds for the variables.

Because external evaluations can be expensive, solvers schedule these
propagators at low priority.

This chapter describes how to define black-box propagators in a model, and the
two external interfaces a black-box can be implemented against: a
:ref:`dynamically loaded library <blackbox-dll>` and an
:ref:`executable subprocess <blackbox-exec>`.

.. _blackbox-mzn:

Defining a black-box propagator in MiniZinc
-------------------------------------------

The interface is provided by an experimental library that must be included:

.. code-block:: minizinc

  include "experimental/blackbox.mzn";

A black-box propagator is declared as a **predicate or function without a
body**, carrying two annotations: a *kind* annotation that selects the type of
propagator, and a *source* annotation (:mzn:`blackbox_exec` or
:mzn:`blackbox_dll`, see :ref:`blackbox-source`) that says where the external
implementation lives. The MiniZinc compiler generates the body of the
declaration automatically.

The decision-variable arguments of a propagator are restricted to **at most
one** :mzn:`list of var int` and **at most one** :mzn:`list of var float`. Any
number of fixed (parameter) arguments may also be given; these are not
automatically passed to the external function but may be used by the companion
functions described below.

Value propagators
~~~~~~~~~~~~~~~~~~

A **relational** value propagator is a predicate. It is evaluated only once all
of its variables are fixed and reports whether the assignment satisfies the
constraint:

.. code-block:: minizinc

  predicate all_ok(list of var int: xs)
      ::minizinc_value_propagator
      ::blackbox_exec("./checker");

  constraint all_ok(x);

A **functional** value propagator is a function returning :mzn:`list of var int`
or :mzn:`list of var float`; the external function computes the output values
rather than merely checking them. Because the length of the result cannot be
known in general, a functional propagator must also provide a ``par`` overload
of the same function which the compiler evaluates to determine the number of
elements in the output:

.. code-block:: minizinc

  % par overload: the propagation function, used to size the output
  function list of int: transform(list of int: xs) =
      [ xs[i] + i | i in index_set(xs) ];

  % the black-box declaration (no body)
  function list of var int: transform(list of var int: xs)
      ::minizinc_value_propagator
      ::blackbox_dll("libtransform.so");

  constraint y = transform(x);

Bounds propagators
~~~~~~~~~~~~~~~~~~~

A **bounds** propagator is a predicate that receives the current bounds of its
variables and returns tightened bounds:

.. code-block:: minizinc

  predicate my_bounds(list of var int: xs)
      ::minizinc_bounds_propagator
      ::blackbox_exec("./bounds");

  constraint my_bounds(x);

Lazy clause generation solvers require a *reason* for every bound change. By
default the propagator uses a conservative reason in which every input bound
explains every output bound. Tighter explanations can be provided by defining a
companion ``_reason`` function, evaluated once during compilation, that returns
for each variable index the literals explaining its lower- and upper-bound
changes (using ``PR_LB`` / ``PR_UB`` of the ``PropBnd`` enum):

.. code-block:: minizinc

  function list of tuple(int,
                         list of tuple(int, PropBnd),
                         list of tuple(int, PropBnd)):
      my_bounds_reason(list of var int: xs) =
      [ (i, [(j, PR_UB) | j in index_set(xs) where j != i],
            [(j, PR_LB) | j in index_set(xs) where j != i])
      | i in index_set(xs) ];

The reason template is reused during search, so it must be a sound
over-approximation for all reachable domains.

Besides explaining propagated changes, the ``_reason`` template also identifies
exactly which input bounds the propagator inspects. A solver can use this to
schedule the propagator more precisely, waking it only when one of those bounds
changes rather than on every change to an input bound.

.. note::

  A propagator-kind annotation must be paired with a :mzn:`blackbox_exec` or
  :mzn:`blackbox_dll` source annotation that provides the external
  implementation.

.. _blackbox-source:

Selecting the external implementation
-------------------------------------

Two annotations select how the black-box is executed:

.. code-block:: minizinc

  annotation blackbox_exec(string: executable, list of string: args);
  annotation blackbox_dll(string: library, list of string: args);

* :mzn:`blackbox_exec` runs an external program (see :ref:`blackbox-exec`). The
  program is launched directly (no shell); ``args`` are appended to its command
  line.
* :mzn:`blackbox_dll` loads a dynamically loaded library (see :ref:`blackbox-dll`); ``args``
  are passed to the library's ``fzn_init`` function.

For convenience a single-argument form is also available and is equivalent to
passing an empty argument list:

.. code-block:: minizinc

  predicate p(list of var int: xs)
      ::minizinc_value_propagator ::blackbox_exec("./prog");          % no args
  predicate q(list of var int: xs)
      ::minizinc_value_propagator ::blackbox_exec("./prog", ["-v"]);  % with args

.. _blackbox-resolution:

Locating the executable or library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The model can refer to the executable or library by a plain, platform-independent
name (without an extension and, for libraries, without the ``lib`` prefix). When
compiling to FlatZinc, MiniZinc resolves that name to an **absolute path** on the
compilation host and stores it in the generated annotation, so the FlatZinc is
self-contained. Resolution applies the platform naming convention
(``lib<name>.so`` / ``lib<name>.dylib`` / ``<name>.dll``) and searches, in order:

* the directory of the file that declares the propagator (so a helper shipped
  alongside a model or a reusable library is found);
* for executables, the ``PATH``; for libraries, the dynamic-loader search path
  (``LD_LIBRARY_PATH`` / ``DYLD_LIBRARY_PATH`` / ``PATH``) and the usual system
  library directories.

A name that is already an absolute path, or that contains a directory separator,
is used as given (resolved relative to the declaring file). If the target cannot
be found, the name is left unchanged. The solver can still apply its own
resolving at runtime when it loads the library or launches the process.

.. _blackbox-interface:

The external interface
----------------------

Regardless of the mode, a black-box implementation is a single function mapping
input values to output values. The meaning of the arrays depends on the kind of
propagator:

.. list-table::
  :header-rows: 1
  :widths: 20 40 40

  * - Propagator
    - Input
    - Output
  * - value, relational
    - the assigned value of each input variable
    - a single integer: ``1`` if the constraint holds, ``0`` if it is violated
  * - value, functional
    - the assigned value of each input variable
    - the computed value of each output variable
  * - bounds
    - two entries per variable, a lower then an upper bound
      (``lb0, ub0, lb1, ub1, ...``)
    - two entries per variable, the tightened lower and upper bounds

Integer variables use the integer channel and float variables the float
channel. When a propagator has no variables of one type, that channel is empty.

.. note::

  **Concurrency.** By default, most solvers evaluate propagators from a single
  thread, but a solver running in parallel (for example with the ``-p`` flag)
  may evaluate a black box from several workers at once. The two modes handle
  this differently:

  * In **subprocess mode** the solver starts a separate process for each worker,
    so each process only ever handles one request at a time and needs no internal
    synchronisation.
  * In **dynamically loaded library mode** the solver gives each worker its own *instance*
    created using ``fzn_clone`` (see :ref:`below <blackbox-dll-instances>`).
    ``fzn_blackbox`` is therefore only ever called on a given instance by one
    thread at a time, while different instances may run concurrently.

.. _blackbox-dll:

Dynamically loaded library (DLL) mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the black-box is a dynamically loaded library, the solver loads it (e.g.
with ``dlopen``) and resolves the symbol ``fzn_blackbox``:

.. code-block:: c

  void fzn_blackbox(
    void          *instance,
    const int64_t *int_in,    size_t int_in_size,
    const double  *float_in,  size_t float_in_size,
    int64_t       *int_out,   size_t int_out_size,
    double        *float_out, size_t float_out_size
  );

The function is called once per propagation: the solver populates the input
arrays and the function writes its result into the (pre-sized) output arrays,
following the encoding in the table above. ``instance`` identifies the per-use
state the call belongs to (see below); a stateless library can ignore it.

On Windows the entry points use the ``__stdcall`` calling convention and must be
exported explicitly (for example with ``__declspec(dllexport)``, otherwise
``cl /LD`` exports no symbols); on other platforms the ordinary C calling
convention is used. A mismatched convention corrupts the stack on 32-bit
Windows. The templates below wrap this in a small macro (in Rust, the
equivalent is ``extern "system"``).

.. _blackbox-dll-instances:

Instances
^^^^^^^^^

Each dynamically loaded library has a single global namespace, so per-use state
cannot live in global memory. Distinct constraints, and concurrent usage would
clobber one another. As such, we capture such state in an *instance*, for each
district constraint concurrent usage, by exporting three optional functions:

.. code-block:: c

  void *fzn_init (const char **args, size_t args_size);
  void *fzn_clone(void *instance);
  void  fzn_free (void *instance);

* ``fzn_init`` is called once per black-box constraint, with the arguments from
  the :mzn:`blackbox_dll` annotation (an ``argv``-style list, possibly empty).
  It returns an opaque **instance** pointer that is passed to every
  ``fzn_blackbox`` call for that constraint. It can, for example, be use it to
  parse the arguments, load a program, or allocate working state.
* ``fzn_clone`` returns an **independent** copy of an instance, behaving
  identically to its source. The solver calls it to give each parallel worker its
  own instance, so that concurrent ``fzn_blackbox`` calls never share one.
* ``fzn_free`` releases an instance. Every pointer returned by ``fzn_init`` or
  ``fzn_clone`` is freed exactly once.

If the library does not export ``fzn_init`` the instance is ``NULL`` and
initialisation is skipped. A library that exports ``fzn_init`` must also export
``fzn_clone``. A **thread-safe** library that keeps no mutable per-call state
need not copy anything: it can **reference-count** the state across ``fzn_init``
/ ``fzn_clone`` / ``fzn_free`` and simply return the *same* pointer from
``fzn_clone`` (bumping the count), freeing the state only when the last
reference is released. All workers then share one instance with no duplication,
while still using the instance-based interface.

Templates: :download:`C <examples/blackbox/blackbox_dll.c>`,
:download:`Rust <examples/blackbox/blackbox_dll.rs>`.

.. _blackbox-exec:

Executable (subprocess) mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In subprocess mode the solver starts the program once (with the arguments from
the annotation) and then communicates over the program's standard input and
output streams, enabling language-agnostic implementations. Each propagation is
a single request line followed by a single response line:

.. code-block:: none

  request : <int values, comma-separated>;<float values, comma-separated>\n
  response: <int outputs, comma-separated>;<float outputs, comma-separated>\n

The integer values and floating-point values are comma-separated, the integer
and floating-point segments are separated by a semicolon, and each message is
terminated by a newline. A segment is empty when there are no values of that
type. For example, sending the integers ``5`` and ``-7`` together with the
floating-point values ``2.5`` and ``1.125`` is written as:

.. code-block:: none

  5,-7;2.5,1.125\n

For a bounds propagator each segment carries two entries per variable (a lower
then an upper bound), following the encoding in the table above.

The number of output values in the stream must match the number of output
variables in the constrain call. If the response does not contain exactly that
many values, this should be considered a runtime error.

The program must:

* read one request line, compute its response, and write exactly one response
  line, in the same format;
* **flush** its standard output after each response;
* keep looping until standard input reaches end-of-file, then exit.

Anything the program writes to its standard error stream is ignored by the
solver and may be used for logging.

Templates: :download:`C <examples/blackbox/blackbox_exec.c>`,
:download:`Rust <examples/blackbox/blackbox_exec.rs>`,
:download:`Python <examples/blackbox/blackbox_exec.py>`.

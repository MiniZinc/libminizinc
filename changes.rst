MiniZinc Change Log
-------------------

For detailed bug reports consult the issue tracker at
https://github.com/MiniZinc/libminizinc/issues.

.. _v2.9.3:

`Version 2.9.3 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.9.3>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 23 May 2025)

Changes:
^^^^^^^^
-  Add ``mzn_max_version_required`` to enable models to speficy the maximum
   version of MiniZinc required to run the model (:bugref:`872`).
-  Replace use of ``ptrdiff_t`` with appropriate unsigned integer type.
-  Clarify documentation example use of boolean extra flags in solver
   configuration files.
-  Warnings produced in solution checkers are now output as part of the
   ``checker`` message when running in ``--json-stream`` mode.

Bug fixes:
^^^^^^^^^^
-  Fix the rewriting of the multidimensional search annotations to ensure
   correct coercion of arguments (:bugref:`897`).
-  Output location of some errors where the problematic expression previously
   did not have a location (:bugref:`899`).
-  Fix domain computation to avoid variables being moved to the output model
   when they are constrained (:bugref:`911`).
-  Fix output processing to also respect the ``-o`` flag when flattening already
   detects unsatisfiability (:bugref:`908`).
-  Report an error when solving a FlatZinc file that does not contain a solve
   item (:bugref:`907`).
-  Pass command line options to the compiler phases earlier to avoid problems
   with e.g. the ``--disable-warnings`` flag (:bugref:`893`).
-  Fix incorrect generation of par versions of functions referencing top-level
   tuples/records containing var fields (:bugref:`919`).
-  Fix problem where using an iterator of variable tuple or record types could
   result in a segfault (:bugref:`901`).
-  Fix a problem where an operator such as ``<=`` on an optional type would
   sometimes lead to an internal compiler error (:bugref:`898`).
-  Print enum values in stack traces (:bugref:`912`).
-  Fix a bug where incompatible overloads could be used when dispatching to more
   specific versions of functions (:bugref:`905`).
-  Fix internal error when constraining the domain of ``var opt bool``
   variables.
-  Fix record merge operator when evaluated via a field access (:bugref:`892`).
-  Update documentation to indicate that the  ``float_ceil``, ``float_floor``,
   and ``float_round`` FlatZinc builtins need to be placed in
   ``redefinitions-2.7.1.mzn`` (:bugref:`914`).
-  Fix handling of basic standard flags when running ``.fzn`` files
   (:bugref:`920`).
-  Fix return type computation for generic functions called with tuple or record
   types (:bugref:`902`).
-  Fix type computation for arrays of tuples/records containing ``$T`` members.
-  Fix flattening of anonymous variables of enum type to be correctly bounded.
-  Emit type errors for unsupported usage of anonymous variables in tuples and
   records. 
-  Fix incorrect flattening of ``in`` for tuples/records containing optional
   values (:bugref:`900`).

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^
-  Suppress warnings generated when running compiled solution checkers.

.. _v2.9.2:

`Version 2.9.2 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.9.2>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 6 March 2025)

Bug fixes:
^^^^^^^^^^
-  Prevent output of and emit warning for invalid statistics in JSON streaming
   mode.
-  Fix crash in string interpolation (:bugref:`891`).
-  Fix ``row`` and ``col`` functions to use enumerated types instead of
   ``int`` (:bugref:`888`).

.. _v2.9.1:

`Version 2.9.1 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.9.1>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 3 March 2025)

Changes:
^^^^^^^^
-  Don't include empty stack traces in JSON stream errors or warnings
   (:bugref:`880`, :idebugref:`212`).
-  Wait 1 second between sending ``SIGTERM`` and ``SIGKILL`` signals to solvers
   to allow more time for cleanup (:bugref:`882`).

Bug fixes:
^^^^^^^^^^
-  Fix a bug where calls to ``enum2int`` weren't correctly removed in
   comprehensions, leading to an internal error (:bugref:`879`).
-  Fix false-positive implicit enum coercion warnings when calling ``card`` and
   some inequality operators.
-  Fix false-positive implicit enum coercion warnings for calls with
   comprehension arguments (:bugref:`887`).
-  Recursively type check type-insts of variable declarations and function
   items.

   -  This enables enum constructors and other functions to be used on the
      left hand side of declarations, and allows overloading of functions
      with identifiers in type-insts to be correctly resolved.

-  Fix type error in flexible job shop documentation example caused by change
   of signature of ``cumulative`` to enum-safe version in 2.9.0.
-  Accept enum types for the two argument ``regular`` constraint to avoid
   false-positive warnings about implicit enum to int coercion.
-  Disallow multiple comma-separated arguments in string interpolations,
   which previously caused unexpected behaviour.
-  Fix incorrect output of non-improving solutions when using HiGHS.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^
-  Use scroll buttons when the tab bar overflows to ensure the side panels can
   be resized (:idebugref:`213`).

.. _v2.9.0:

`Version 2.9.0 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.9.0>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 11 February 2025)

Changes:
^^^^^^^^
-  Add support for ``elseif`` in ``if then elseif endif`` expressions without an
   `else` branch.
-  Make ``sort`` functions return array of enum when input is array of
   enum (:bugref:`853`).
-  Make the ``mzn_in_root_context`` function (which is used only internally and
   should not be used in user models) more flexible in where it can be called.
-  Update packaged version of the HiGHS solver to version 1.8.1.
-  Update Gurobi interface to be compatible with Gurobi 12.0.0.
-  Address compiler warnings concerning unsafe signed/unsigned comparisons and
   implicit casts between different integer types.
-  **BREAKING**: The usage of ``--compile`` (or ``-c``) flag now requires the
   user to explicitly specify the solver for which the MiniZinc instance is being
   compiled, using ``--solver``. To select the default solver (and keep the
   previous behaviour) you can use ``--solver default``.
-  Add the ``--fzn-format`` flag to influence whether the generated FlatZinc from
   ``--compile`` or ``--fzn`` is given in the traditional FlatZinc format or the
   new JSON-based format. (:bugref:`868`)
-  Comments regarding the compilation invocation are now included in user-facing
   FlatZinc (``.fzn``) files, created by the ``--compile`` or ``--fzn`` flags.
-  Add ``par`` version of the ``among`` function.
-  Add support for arrays containing arrays (which only support access using
   ``par`` indices).
-  Add ``diversity.mzn`` library to be used with the diverse solutions toolchain
   included in MiniZinc Python.
-  Ensure the MiniZinc directory is added to the DLL search path when running
   child processes on Windows (:idebugref:`206`).
-  Add ``par`` implementations for the ``arg_val`` and ``arg_val_weak`` functions.
-  Add support for calling enum constructors without arguments to return the set
   of all constructed values.
-  Deprecate and emit warning for implicit coercion of enums to integers. The
   ``enum2int`` function should be used to perform the coercion where required.

Bug fixes:
^^^^^^^^^^
-  Fix boolean context when flattening tuple or record literals containing
   booleans.
-  Fix memory leak in overflow handler.
-  Fix crash when calling ``outputJSON`` (:bugref:`856`).
-  Fix incorrect typing of arrays in assignment generators (:bugref:`858`).
-  Fix bug in flattening of implication with optional right hand side.
-  Fix flattening of ``exists`` and ``forall`` with optional arguments.
-  Fix handling of absent values when removing unused code (:bugref:`864`).
-  Fix compilation of empty arrays with empty domains (:bugref:`860`).
-  Fix evaluation of ``dom_array`` on par arrays (:bugref:`851`).
-  Fix flattening of array slices inside tuples and records (:bugref:`859`).
-  Fix defines_var annotation for cyclic and missing definitions (:bugref:`863`).
-  Fix assertion failures when using arrays as argument to bin packing constraints
   (:bugref:`865`).
-  Fix operator precedences for ``intersect`` and unary plus/minus.
-  Fix crash when type instance concatenation is performed on invalid types
   (:bugref:`867`).
-  Fix a segfault caused by an internal `float_times` variant not correctly being
   handled during output processing (:bugref:`870`).
-  Fix incorrect non-uniform if-then-else type error when one branch is an empty
   array and another branch is an array of tuples or records.
-  Fix type checking for `any`` in `let` expressions with tuple types.
-  Fix pretty printing of variable declarations with `any` type.
-  Fix bug where the JSON parser would not add absent literals for known optional
	 types that are missing from the JSON objects.

.. _v2.8.7:

`Version 2.8.7 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.8.7>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 2 October 2024)

Bug fixes:
^^^^^^^^^^
-  Fix bug that caused where clauses to be removed from comprehensions.
-  Fix optimisation of Boolean constraints that could sometimes trigger an
   assertion failure in debug builds (:bugref:`843`).

.. _v2.8.6:

`Version 2.8.6 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.8.6>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 25 September 2024)

Changes:
^^^^^^^^
-  Variable ``in`` operators on arrays of (nested) records and tuples containing
   only enumerable (enum, int, or bool) will now be rewritten into ``table``
   global constraints.

   -  Values that are ``par`` in the left hand side will be checked and filtered
      from the resulting table constraint.

-  Variable ``<``, ``<=``, ``>``  and ``>=`` operators on (nested) records and
   tuples containing only enumerable (enum, int, or bool) will now be rewritten
   into ``lex_less`` or ``lex_lesseq`` global constraints.
-  Automatically detect SCIP 9.0.1 and potential future versions on Windows.
-  The interface to the HiGHS solver now requires version 1.7.2 or later.
-  Enforce strict enum type correctness for set operations (:bugref:`828`).
-  Add ``par opt`` overloads of ``min`` / ``max`` and return enum values instead
   of coercing to integers.
-  Use half-reification only when there isn't a reification that is a more
   specific match.
-  Add multidimensional overloads for the ``++`` array concatenation operator.

Bug fixes:
^^^^^^^^^^
-  Fix incorrect type error for array literals where the first element is ``<>``
   and subsequent elements are sets (:bugref:`818`).
-  Fix missing code block in visualisation feature documentation.
-  Emit type error when using variable if-then-else/array access containing
   tuples or records containing non-varifiable types.
-  Emit type error when using a variable comprehension with a tuple/record type.
-  Emit type error when using optional array access for arrays of
   tuples/records.
-  Emit type error for array literals with ``<>`` and var set elements.
-  Fix missing cross-references for IDE preference dialog documentation figures.
-  Fix crash when using nested tuples/record types inside ``var`` if-then-else
   expressions.
-  Fix evaluation error for comprehensions with an initial assignment generator
   and a where clause which doesn't involve any of the generator identifiers.
-  Fix type checking of if then else expressions with ``_`` in the else branch
   (:bugref:`821`).
-  Fix parsing of the most negative integer literal (:bugref:`822`).
-  Fix typo in warning messages for undefined results in function calls.
-  Fix bug in flattening identifier causing debug assertion to fail
   (:bugref:`826`).
-  Fix missing type when flattening indexed var comprehension leading to error
   or incorrect value type.
-  Fix crash when generating error message for mismatched index set where an
   enum index set is empty.
-  Fix crash due to incorrect handling of negated constraints matching entry in
   CSE map (:bugref:`832`).
-  Fix type error due to missing overload of ``max`` (:bugref:`848`).
-  Fix handling of contexts and ``maybe_partial`` annotations when flattening
   par expressions.
-  Fix par evaluation of boolean operators where an operand is an array access
   into an empty array literal.
-  Fix crash when concatenating arrays of annotations (:bugref:`842`).
-  Ensure DZN output of enum index sets of multidimensional arrays is properly
   quoted.
-  Fix parsing of enumerated values in JSON for integer parameters.
-  Fix crash during flattening of clauses with opt bool arguments
   (:bugref:`845`).
-  Fix the coercions of index set for empty array literals to allow any of the
   index sets to be empty (:bugref:`844`).
-  Fix behaviour of overloading for the ``++`` operator (:bugref:`840`).
-  Avoid outputting ``objective`` statistic when value is invalid for MIP
   solvers (:bugref:`841`).
-  Fix bug in unification of aliased output variables when using ``-O2`` and
   above.

.. _v2.8.5:

`Version 2.8.5 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.8.5>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 3 June 2024)

Bug fixes:
^^^^^^^^^^
-  Fix specification for constraint items and annotations in FlatZinc JSON.
-  Fix flattening of negated let expressions with constraints.
-  Fix crash when equating identifiers to tuples/records in if-then-else
   expressions.
-  Fix ``string_length`` function to return the number of Unicode code points
   rather than the number of bytes (i.e., to interpret the string as UTF-8).
-  Emit type error for opt tuples and records created through type aliases.
-  Fix evaluation of par partial functions returning arrays.
-  Fix type inference for if-then-else expressions with different tuple or
   record types in the branches.
-  Fix evaluation error caused by removal of fixed FlatZinc variables without
   fixing them in the output model.
-  Fix computed type when flattening cv comprehensions containing tuple or
   record types.
-  Fix unsatisfiability caused by reification in negated boolean context
   (:bugref:`813`).
-  Fix bug in computation of common type of incompatible record types.
-  Fix crash when type checking nested arrays of tuples or records.
-  Fix incorrect unification of flattened tuple/record fields with paths
   enabled.

Changes:
^^^^^^^^
-  Add ``string_split`` function that splits a string into an array of strings
   based on a separator.
-  Add ``json_section`` annotation to mark an output item as a JSON section
   (:bugref:`816`).

.. _v2.8.4:

`Version 2.8.4 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.8.4>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 10 May 2024)

Changes:
^^^^^^^^
-  Add ``-w`` and ``--disable-warnings`` flags that will suppress all warnings
   emitted by the flattening process.
-  Add option for solver libraries to set ``mzn_opt_annotate_computed_domains``
   such that the compiler will signal domains that are enforced by separate
   constraints using a ``computed_domain`` annotation.
-  Always output multidimensional arrays as nested arrays when using
   ``showJSON`` for consistency.
-  Add missing implementation for ``default`` for sets expressions.
-  Add missing optional versions of unary mathematical functions/operators.
-  Improve special-case handling of boolean ``<->`` and ``xor`` operators.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^
-  Fix bundled OpenSSL libraries on Windows.
-  Allow MOOC submission window to be scrolled.
-  Increase hard process termination timeout to 1s.

Bug fixes:
^^^^^^^^^^
-  Fix crash when using records inside solution checker modules.
-  Fix pretty printing of record literals and types with unicode field names.
-  Fix creation of par versions functions containing ``var`` binary/unary
   operators (:bugref:`792`).
-  Fix potential crash when printing of output model involving records.
-  Fix computation of supertype of tuples/records involving ``<>`` fields.
-  Fix a bug where the compiler would incorrectly set variable domains as
   computed when processing linear constraints.
-  Fix invalid JSON produced by MIP interfaces due to infinite values for
   statistics.
-  Fix possible crash during type specialisation involving ``array [_] of $$T``
   types.
-  Fix output of structured types containing arrays of enums (:bugref:`793`).
-  Fix justified formatting to work with arrays of enums.
-  Fix incorrect unification of arrays of reverse mapped variables with paths
   enabled.
-  Fix problem where incorrect type errors would occur when merging records or
   tuples types in an array declaration.
-  Fix bug where unused reverse-mapped variables were removed from the output
   model while still being marked as output in the FlatZinc.
-  Fix decomposition of set variables with empty domains when using
   ``nosets.mzn``.
-  Fix a problem where the reduced range syntax in combination with enums would
   produce an error when the resulting range was empty.
-  Fix incorrect unification of tuple/record fields in array accesses with paths
   enabled (:bugref:`802`).
-  Fix crash when using tuples/records in par arrays or calls containing var
   expressions.
-  Fix bug where variables could be removed from the FlatZinc but not the output
   model when optimising ``bool2int`` (:bugref:`803`).
-  Add missing coercions for comprehension where clauses and assignment
   generators (:bugref:`807`).
-  Fix bug in handling of domains for structured types during type
   specialisation.
-  Fix handling of search annotations when reading FlatZinc files.
-  Fix bug during optimisation which could cause the compiler to hang due to
   not updating hashes of identifiers (:bugref:`805`).
-  Fix incorrect definition of ``all_different`` in the ``gecode_presolver``
   library where it was not enforced that fixed values be all different
   (:bugref:`798`).
-  Fix crash when resolving type aliases for function parameters with ``$T``
   array dimensions (:bugref:`806`).
-  Fix ``default`` operator for ``par opt set`` parameters (:bugref:`809`).
-  Fix output of ``par opt set`` enumerated types.
-  Fix pretty printing of records when using the document printer.
-  Fix internal error when binding numeric literals to declarations with
   existing right-hand sides.
-  Fix contexts of arguments of double-negated integer comparisons
   (:bugref:`810`).
-  Fix context of indices in array access expressions (:bugref:`719`).

.. _v2.8.3:

`Version 2.8.3 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.8.3>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 1 February 2024)

Bug fixes:
^^^^^^^^^^
-  Fix problem where specialised flattening for some forms of binary operators
   would not correctly forward the partiality of its arguments (:bugref:`766`).
-  Remove internal duplicate functionality that could sometimes cause problems
   when assigning variables in constraints (:bugref:`769`).
-  Do not try to check right hand side of parameter variable declarations that
   are ``par`` because of singular domains (:bugref:`771`).
-  Fix a problem in the parser where anonymous enumerated types could not be
   used in DZN files (:bugref:`776`).
-  Fix identifier lookup bug in internal Chuffed interface (:bugref:`782`).
-  Fix some minor inconsistencies in the FlatZinc JSON schema and generation.
-  Fix crash involving ``var`` comprehensions with set types by rejecting
   unsupported type (:bugref:`779`).
-  Fix internal error during evaluation of ``par opt set`` expressions.
-  Fix invalid FlatZinc produced when search annotations are passed non-1-based
   arrays (:bugref:`768`).
-  Fix bug in evaluation of annotations causing invalid FlatZinc to be produced
   (:bugref:`768`, :bugref:`788`).
-  Fix substitution of fixed objective variable value into solve item
   (:bugref:`773`).
-  Fix check for presence of ``set2iter`` redefinition (:bugref:`778`).
-  Fix incorrect removal of constraint due to use of redefinition of
   ``bool_clause_reif`` when constraint is not reified (:bugref:`785`).

Changes:
^^^^^^^^
-  Remove documentation of ``strictly_increasing`` (and decreasing) for ``bool``
   types, and change definition from a solver redefined global, to a simple
   library definition with user warning (:bugref:`784`).
-  Add version of ``strictly_increasing`` (and decreasing) for ``float``, ``opt
   float``, and ``set of int`` types.
-  Automatically detect SCIP 8.1.0 and potential future versions on Windows.
-  Fix crash when using type aliases with ``-O2`` (:bugref:`783`).
-  Automatically detect Gurobi 11.0 and potential future versions.
-  Allow array literals with all ``<>`` members (:bugref:`747`).

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^
-  Increase maximum number of threads from default Qt limit (:idebugref:`196`).

.. _v2.8.2:

`Version 2.8.2 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.8.2>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 15 December 2023)

Bug fixes:
^^^^^^^^^^
-  Fix incorrect FlatZinc definition in the ``gecode_presolver`` solver library (:bugref:`755`).
-  Fix type specialisation for function with an polymorphic array argument and a
   non-array return type with the same type-inst identifier (related to :bugref:`671`).
-  Fix crash during typechecking of overloaded functions differing only by inst
   returning structured types.
-  Fix incorrect type checking of polymorphic functions with type-inst IDs
   inside structured types.
-  Fix evaluation error when boolean variables are fixed during flattening
   (:bugref:`758`).
-  Fix incorrect call names output when generating ``.ozn`` files
   (:bugref:`759`).
-  Fix incorrect output of record access as tuple access in ``.ozn`` files
   (:bugref:`759`).
-  Fix definition of ``array_intersect`` so that it can be used in non-positive
   contexts.
-  Fix standards definitions of ``increasing`` variants to correct be ignored
   when the argument array is empty (:bugref:`762`).
-  Fix a problem where exceptions thrown for undefined expressions were not
   caught during the aggregation of linear expressions, breaking relational
   semantics (:bugref:`760`).
-  Fix crash when calculating computed domain of a declaration (:bugref:`765`).
-  Fix x-y line/scatter visualisations.
-  Fix `show_array2d_bool` for index set containing negative or zero (:bugref:`780`).

Changes:
^^^^^^^^
-  Support ``var set``s of enums for ``array_union`` and ``array_intersect``.
-  Support ``var`` enums for the ``..`` range operator.
-  Always perform substitution of fixed values before outputting FlatZinc for
   consistency.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^
-  Make process handling more robust to failures.

.. _v2.8.1:

`Version 2.8.1 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.8.1>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 27 November 2023)

Bug fixes:
^^^^^^^^^^
-  Fix incorrect message for par array out of bounds indicating that array is empty
-  Fix incorrect propagation of Boolean variables potentially causing output not
   accepted by Gecode and Chuffed (:bugref:`748`).
-  Fix a problem where the usage of ``lb`` on a Boolean expression would return
   ``-infinity``. (:bugref:`749`).
-  Fix omission of error location when there is no stack trace available.
-  Fix type specialisation to always make par versions of functions available
   for output (:bugref:`751`).
-  Fix internal error when checking return value of functions involving arrays
   of optional values (:bugref:`752`).
-  Fix incorrect ``false`` values for ``has_output_item`` when running with
   ``--model-interface-only``
-  Fix translation of search annotations with multi-dimensional arrays as arguments
   (:bugref:`750`).
-  Fix bug in output generation for some visualisation functions.
-  Fix problem where tuple or record assignments would sometimes trigger
   segmentation faults.
-  Fix context when binding the result of flattening a concatenation operation,
   resolving a possible segmentation fault (:bugref:`754`).
-  Fix incorrect possible evaluation error for ``in`` operator involving an
   array RHS.

Changes:
^^^^^^^^
-  Add ``--solution-checker`` option to allow specifying the solution checker
   (allowing use from a parameter configuration ``..mpc`` file).
-  Produce tighter bounds in ``compute_mod_bounds`` for the ``mod`` operator.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^
-  Fix command used to run findMUS and Globalizer.
-  Add ability to set the ports used for the visualisation server.
-  Add option for printing the visualisation server URL for debugging purposes.
-  Add more information to subprocess error messages.

.. _v2.8.0:

`Version 2.8.0 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.8.0>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 16 November 2023)

Changes in interfaces to solvers:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
-  `OR-Tools <https://developers.google.com/optimization>`_ is now bundled with
   the MiniZinc IDE on all platforms.
-  HiGHS is now loaded as a dynamic plugin instead of statically linked, and
   now supports outputting intermediate solutions for optimisation problems.
-  Add support for producing a JSON-based version of FlatZinc as the output
   format of the MiniZinc compiler.
-  Replace ``supportsMzn``, ``supportsFzn`` and ``supportsNL`` solver
   configuration flags with new option ``inputType``. The old flags are
   still supported for backwards compatibility.
-  Add *experimental* support for restart based meta-search specification for
   the Gecode and Chuffed solvers, as first explored in `Solver-Independent
   Large Neighbourhood Search <https://doi.org/10.1007/978-3-319-98334-9_6>`_
   and `A Modern Architecture for Constraint Modelling Languages
   <https://doi.org/10.26180/16968229.v1>`_.
-  Automatically detect current versions of CPLEX (:bugref:`745`).

Changes in the MiniZinc Library:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
-  Add ``cumulatives`` scheduling global constraint.
-  Add the ``opt`` variants of the ``global_cardinality`` functions, and add
   enumerated type typesafety between the values of the decision variables and
   the cover parameters.
-  Add optional versions of ``count_*`` global constraints (:bugref:`728`).
-  ``(strictly_)decreasing`` will now always be rewritten into
   ``(strictly_)increasing`` and has support for option types.
-  Allow libraries to define how to iterate over ``var set of int`` using the
   function ``set2iter``, useful for different set representations.
-  Stabilise the IDE visualisation library, allowing all visualisation functions
   to be used from ``output`` statements, and removing the need for manual calls
   to ``showJSON`` in custom visualisations. This is a breaking change for users
   of the previous experimental API.
-  Add ``mzn_half_reify_clause`` compiler option to allow solvers to disable the
   half reification of ``bool_clause`` constraints.
-  Update the reflection functions ``ub``, ``lb``, and ``dom`` to return enum
   values.
-  Use tuples to implement the decomposition of optional variables, avoiding
   possible CSE aliasing problems.

Changes in the compiler
^^^^^^^^^^^^^^^^^^^^^^^
-  CSE has been adjusted to handle commutative functions when annotated using
   the ``promise_commutative`` annotation.
-  ``mzn_reverse_map_var`` is now only called on output variables that do not
   yet have reverse mappers after initial flattening, but are required by the
   output model.
-  Improve error messaging for invalid parameter configuration (``.mpc``) files.
-  Add a list of messages generated by solution checkers to the ``checker``
   message when using ``--json-stream``.
-  Support output of command line argument parsing errors in ``--json-stream``
   mode.

Bug fixes:
^^^^^^^^^^
-  Fix restoration of tuple/record domains when flattening ``let`` expressions.
-  Fix type checking error due to creation of incorrect par versions of
   functions involving tuples/records.
-  Ensure that when ``--solver <FILE.msc>`` is used, the given solver
   configuration file is always selected even when it collides with a solver
   in the search paths (:bugref:`715`).
-  Fix error when running satisfaction problems using the Gecode presolver
   where an output variable is also an introduced variable.
-  Resolve a problem where unification in the optimization phase might remove
   variables part of the output (:bugref:`716`).
-  Fix possible crash when printing the final solution using the built-in
   Chuffed interface.
-  Don't print the final status line from solution checkers.
-  Fix typechecking of par type-inst identifiers when instantiated using
   structured types involving ``var`` types.
-  Implement ``fix`` and ``is_fixed`` for structured types.
-  Ensure reverse mappers are created when flattening tuple/record literals.
   This resolves certain errors during output processing in models using these
   types.
-  Fix problem where certain strings in exceptions might be garbage collected
   before they are output (:bugref:`725`).
-  Fix problem where argument names of generated functions could conflict with
   model declared names (:bugref:`726`).
-  Fix problem where the common type of complex records or tuples was not
   correctly determined in the array literals.
-  Fix a problem in the parser where a nullptr would be used before a syntax
   error was thrown (:bugref:`730`).
-  Fix error management when reading preference files (:bugref:`729`).
-  Fix segmentation fault caused by the creation of invalid domain constraints
   generated for functions with arrays of tuples or records parameters
   (:bugref:`732`).
-  Fix crash when instantiating a type-inst identifier using only ``<>``.
-  Fix evaluation of comprehensions containing ``opt string``.
-  Fix crash when instantiating polymorphic functions using tuples/records with
   many ``var`` or ``opt`` fields.
-  Do not generate default DZN output when there are only sectioned ``output``
   items present (:bugref:`733`).
-  Fix the edge indexing of the edges for the final node in the ``neural_net``
   global constraint (:bugref:`738` :bugref:`573`).
-  Add better error messaging when an index set mismatch is encountered in the
   standard library (:bugref:`740`).
-  Fix evaluation error when indexing into an array using ``<>``.
-  Fix incorrect unsatisfiability when equating optional variables with
   non-intersecting domains.
-  Fix array access using optional indices for multidimensional arrays.
-  Fix output of zero length arrays of optional variables.
-  Fix output processing performance degradation when printing arrays in DZN
   output mode.
-  Fix card function on set of floats and bools.
-  Make set literals containing strings or annotations a type error.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^
-  Fix unreadable cheat sheet font colour in dark mode (:idebugref:`191`).
-  Add option to output objective value and enable by default.
-  Show manually input parameters in output window.
-  Fix missing checker messages (:idebugref:`192`).
-  Fix incorrect OpenSSL version in Linux packages (:idebugref:`189`).

.. _v2.7.6:

`Version 2.7.6 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.7.6>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 20 June 2023)

Changes:
^^^^^^^^

-  Change the library and solver configuration path of builtin version of
   Chuffed to prevent file conflicts (:bugref:`708`).
-  Use the verbose compilation flag to control the verbosity of the MIP domains
   stage (:bugref:`705`).

Bug fixes:
^^^^^^^^^^

-  Resolve issue where right hand assignment of set types could potentially be
   replaced by literal value (:bugref:`700`).
-  Fix incorrect undefinedness of element access of arrays of empty sets.
-  Fix possible incorrect type error when calling an overloaded function with
   an argument whose true type after flattening causes ambiguity when finding a
   better match.
-  Accept ``--num-solutions`` rather than only ``-n`` in the Gecode presolver.
-  Fix crash when using a record type with a field that is itself a record or
   tuple.
-  Fix type error when calling ``show`` on var enum (:bugref:`711`).

.. _v2.7.5:

`Version 2.7.5 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.7.5>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 7 June 2023)

Changes:
^^^^^^^^

-  Introduces the ``arg_val`` and ``arg_val_weak`` globals that return the index
   of the first occurrence of a value in a given array. (:bugref:`544`).
-  Introduces the ``var_perm_sym`` and ``var_sqr_sym`` symmetry breaking
   globals.
-  Enable parallel search for the built-in Gecode presolver.

Bug fixes:
^^^^^^^^^^

-  Resolve problem in the optimization of set variable constraints
   (:bugref:`693`).
-  Fix regression in the context determination of negated binary operators
   (:bugref:`695`).
-  Ensure statistics are output when MiniZinc is interrupted with JSON stream
   output enabled (:bugref:`692`).
-  Add ``par`` version of ``set_in_imp`` to the ``nosets.mzn`` library to
   warnings about out-of-bound indices in non-user models (:bugref:`696`).
-  Add missing bounds calculation for ``symdiff`` (:bugref:`700`).

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Use native Qt dark mode on Windows where supported.
-  Improve behaviour of the custom solver parameter dialog.

.. _v2.7.4:

`Version 2.7.4 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.7.4>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 11 May 2023)

Changes:
^^^^^^^^

-  ``ASTString`` constructor that uses ``std::string`` is now explicit to avoid
   unexpected or unnecessary GC allocation.
-  Circular definitions through the use of annotations will now be reported as
   type errors, rather incurring infinite recursion (:bugref:`680`).
-  Optimize the compilation of identical calls in different contexts by
   recursively changing the contexts of the (uncompiled) dependent calls,
   instead of flattening multiple times.
-  Change domains of set variables when encountering (par) equalities, or
   negated ``in`` constraints at the root level.

Bug fixes:
^^^^^^^^^^

-  Remove redundant function return simplification that could create invalid
   tuple/record types (:bugref:`681`).
-  Fix regression in the creation of separate domain constraints where domains
   contained infinity (:bugref:`684`).
-  Fix a flattening the decompositions of lex_less(eq) reifications in the
   standard library, caused by a missing ``promise_total`` annotation
   (:bugref:`685`).
-  Fix incorrect context being used when flattening some binary operators
   (:bugref:`683`).
-  Fix with problem with domain copies when varifying a type-inst synonyms
   (:bugref:`687`).
-  Add missing support for floating point literals containing exponents in the
   JSON parser (:bugref:`690`).
-  Fix a problem where the compiler would incorrectly set the computed domain
   flag (:bugref:`691`).
-  Resolve garbage collection issue in the typechecker (:bugref:`688`).
-  Add missing error message for sets of records and sets of tuples.

.. _v2.7.3:

`Version 2.7.3 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.7.3>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 20 April 2023)

Changes:
^^^^^^^^

-  Calls to ``min`` and ``max`` with ``par set of int`` type argument will now
   be undefined (becoming ``false`` in the closest Boolean context). This makes
   them consistent with the calls with an argument of type ``var set of int``.
   Note that previous behaviour would have returned ``infinity`` or
   ``-infinity`` respectively (:bugref:`661`).

Bug fixes:
^^^^^^^^^^

-  Fix incorrect handling of enum types during return type specialisation of
   functions (:bugref:`670`).
-  Fix incorrect restriction of domains for arrays containing tuples/records
   (:bugref:`669`).
-  Fix crashes/invalid FlatZinc generated due to incorrect aggregation of
   boolean operators involving optional arguments (:bugref:`668`).
-  Fix handling of absent values when checking the return value of par functions
   against the type-inst (:bugref:`673`).
-  Fix small redundancy in the flat_cv_exp that triggers errors (and assertions)
   in some cases (:bugref:`674`).
-  Fix a mistake in the context when binding the result of builtins returning
   an expression (:bugref:`667`).
-  Fix problem with function domain constraints containing infinity.
-  Fix crash when flattening field access in negative context (:bugref:`675`).
-  Fix crash when flattening variable declaration annotations for tuple
   variables with paths enabled (:bugref:`675`).
-  Fix incorrect ordering of enum parts when using separate assignment items.
-  Simplify fixed arrays by pushing them on the queue, rather than using
   simplify_bool_constraint. Handle boolean variables with fixed domain
   correctly in fixed literal counting (:bugref:`671`).
-  Propagate more Boolean constraints during the compiler optimisation phase
   (:bugref:`676`).
-  Fix the translation of domains of floating point variables that contain
   multiple ranges and include an infinity value.
-  Fix an issue where JSON arrays of sets where parsed incorrectly
   (:bugref:`679`).
-  Fix common subexpression elimination for expressions used in incompatible
   contexts (:bugref:`666`).
-  Add missing ``::promise_total`` to functional version of ``nvalue``.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Only show MOOC error code when response is actually an error
   (:idebugref:`176`).

.. _v2.7.2:

`Version 2.7.2 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.7.2>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 5 April 2023)

Bug fixes:
^^^^^^^^^^

-  Fix crash when specialising a function to return an array with enum index
   sets but integer element type.
-  Fix decompositions for ``ceil`` and ``floor`` functions (:bugref:`657`).
-  Fix incorrect resultant domain when binding identifiers with domains to
   variable declarations (:bugref:`660`).
-  Fix incorrect unknown status given by Chuffed builtin solver interface on
   timeout.
-  Fix segmentation fault that occurred when using some models with checkers
   (:bugref:`662`).
-  Fix incorrect type error when an empty array literal would be used in a
   if-then-else expression with type ``array[_] of set of int`` (:bugref:`664`).
-  Add support for unicode escapes in JSON input strings.
-  Fix possible crash when outputting JSON parsing errors.
-  Fix possible crash during flattening of ``++`` operator.

Changes:
^^^^^^^^

-  Add documentation for ``--input-is-flatzinc`` flag (:bugref:`655`).

.. _v2.7.1:

`Version 2.7.1 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.7.1>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 31 March 2023)

Changes:
^^^^^^^^

-  Add ``ceil``, ``floor`` and ``round`` functions for ``var float``, and
   corresponding FlatZinc builtins ``float_ceil``, ``float_floor`` and
   ``float_round`` that can be overridden by solvers in the new
   ``redefinitions-2.7.1.mzn`` library file (:bugref:`636`).
-  Make deopt on par absent values return undefined instead of error.

Bug fixes:
^^^^^^^^^^

-  Fix incorrect chain compression when Boolean variables had constrained
   domains (:bugref:`638`).
-  Fix incorrect compilation of let expressions in recursive functions
   (:bugref:`637`).
-  Fix the evaluation of ``par`` optional types with a singular value in their
   domain (:bugref:`639`).
-  Fix common subexpression elimination for Boolean expressions that contain an
   undefined result that needs to become false in the Boolean context
   (:bugref:`639`).
-  Fix linear aggregation to only aggregate non-optional values (:bugref:`644`).
-  Fix rewriting to counting constraints to only occur when result is not
   optional (:bugref:`644`).
-  Add missing internal builtin for xor on optional bool variables
   (:bugref:`644`).
-  Fix evaluation of par array access with absent value (:bugref:`644`).
-  Fix potential crash when evaluating bounds of par let expressions that
   contain a variable (:bugref:`646`).
-  Add missing ``par opt int`` versions of ``~+``, ``~-`` and ``~*``
   (:bugref:`644`).
-  Add missing ``count`` function on ``array[_] of var opt bool``
   (:bugref:`640`).
-  Fix potential crash in typechecking of array of record access
   (:bugref:`647`).
-  Fix output of tuples and records that contain enumerated types.
-  Add missing newline to solver comment output when using ``--json-stream``
   (:bugref:`649`).
-  Fix problem where field names of record types could be removed during garbage
   collections.
-  Fix the detection of Gurobi 10 (:bugref:`653`).
-  Fix performance degradation for models with variables that appear in a large
   number of constraints (:bugref:`645`).
-  Fix the definition of ``pow`` involving negative exponents.
-  Ensure errors/warnings do not output mangled function names (:bugref:`648`).
-  Fix the decomposition of optional ``inverse``.
-  Fix various issues with the instantiation of generic functions involving
   tuples or records.
-  Fix the parsing multidimensional arrays containing values of enumerated types
   in JSON.
-  Fix performance degradation for models with variables that appear in a large
   number of constraints (:bugref:`645`).
-  Fix bug in enumerated types with non-contiguous constructor sets.
-  Fix possible crash when simplifying static function bodies.
-  Fix handling of set literals and search annotation calls in built-in Chuffed
   interface.
-  Fix bug where an already defined variable was made the argument of a
   ``defines_var`` annotation for a constraint.
-  Fix handling of domains for tuple and record types.
-  Fix index set checking for tuples and records which contain arrays.
-  Fix the handling of domain and index set expressions in aliases.
-  Fix incorrect coercion of sets into multidimensional arrays (:bugref:`656`).
-  Fix incorrect simplification of Boolean constraints that contain a fixed Boolean
   variable in an annotation.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Fix highlighting of multiline comments starting with ``/*/`` (:idebugref:`172`).
-  Fix bundling of incompatible OpenSSL version in linux packages.
-  Remove support for glibc 2.27 and earlier from AppImage and tarball linux
   packages. The Snap package may be used instead on such systems.

.. _v2.7.0:

`Version 2.7.0 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.7.0>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 23 February 2023)

Major changes:
^^^^^^^^^^^^^^
-  Support for tuple and record types as well as type-inst synonyms has been added.
-  Extended enumerated types can now be constructed from non-contiguous sets.
-  WebAssembly is now fully supported as a target platform.

Changes:
^^^^^^^^
-  The compiler will no longer introduce ``array_bool_or``, only ``bool_clause``.
-  Improve translation of some simple constraints, which avoids introducing
   unnecessary ``int_eq`` (and similar) constraints.
-  Remove more subsumed clauses and foralls in the optimisation phase of the compiler.
-  Report approximate error locations for type errors in function arguments that
   are literals without location information.
-  Many internal structures, such as the CSE table, and the representation of
   variable declarations and calls have been optimised to require less memory.
-  The compiler now accepts absolute and relative paths as arguments of the
   `-G` flag.
-  Functions in the standard library that return arrays of variables are now
   also annotated with ``is_defined_var`` and ``defines_var(x)`` annotations.
-  Revert changes in the ``fzn_count_*`` variants that tried to introduce
   more positive contexts, but inadvertently came at the cost of count aggregation.
-  The Gecode-based pre-solver can now accept ``--backend-flags`` to allow
   processing of options whose names conflict with other flags.
-  The Gecode-based pre-solver is now made available using the ``gecode`` solver
   tag when using the WebAssembly version of MiniZinc.
-  Remove unnecessary file packager patch from WebAssembly build.
-  There is now a version of the ``regular`` constraint that uses enumerated types
   to represent the states of the automaton.
-  Revert change in the standard decomposition of ``lex_less(eq)`` so it no
   longer uses implications which were intended promote half-reification,
   because this introduced free variables.
-  Calls to par functions with constrained arguments now result in an undefined
   value (just like calls to var functions) rather than an abort.

Changes in interfaces to solvers:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  MiniZinc now includes a solver interface for the `HiGHS <https://www.highs.dev>`_
   linear optimisation suite. HiGHS is included by default in the MiniZincIDE
   package, and available using ``--solver highs``.
-  The SCIP interface has been updated and is now compatible with version
   7 and 8 only.
-  A solver's globals library specified in a configuration file will now
   always be passed to the compiler using the ``-G`` flag. This allows users to
   easily override this library from command line when required.
-  Automatically detect Gurobi 10.0 and potential future versions.
-  Avoid loading all solver plugin DLLs on startup, instead loading when required.
-  A built-in solver interface for `Chuffed <https://github.com/chuffed/chuffed>`_
   has been added primarily for use with the WebAssembly version of MiniZinc.
-  The non-FlatZinc interfaces now also accept the ``--free-search`` flag in
   addition to the ``-f`` flag.
-  The SCIP interface will now also try to find the SCIP library files in the
   Homebrew locations.

Bug fixes:
^^^^^^^^^^

-  Fix the parsing of JSON enum constructor functions with integer arguments.
-  Fix the ``..`` operator on variable integers to create sets with tighter bounds.
-  Fix incorrect memory allocation in Gecode interface (:bugref:`618`).
-  Fix an error thrown when the declaration of a polymorphic function is split from
   its definition.
-  Fix assignment items of enumerated types using the ``++`` operator.
-  Fix behaviour of overloading involving polymorphic functions which overlap
   with more specific functions.
-  Fix extended enum constructors with empty enums.
-  Fix a bug that prevented the use of polymorphic optional arrays as function arguments.
-  Generated FlatZinc files are now closed before a solver is called.
-  Disable inlining for one function when creating webassembly output, to prevent the clang compiler
   from exploiting undefined behaviour that our code relies on.
-  Fix Gecode-based presolver to not crash when solving problems with a constant objective, or
   optimisation problems where some variables are not part of the search or output.
-  Fix default search for Gecode-based presolver to be the same as standalone Gecode solver.
-  Fix possible extraneous newline when printing warnings in ``--json-stream`` mode.
-  Allow option types to be declared with empty domains.
-  Fix an error in checking floating point array domains for functions with defined domains.
-  Fix a bug in the evaluation of array comprehensions with option types.
-  Fix type inference for variables declared ``any`` in let expressions.
-  Fix possible crashes during garbage collection of identifiers and variable declarations.
-  Boolean par functions (as declared using ``test``) now behave correctly when they
   contain partial operations that result in undefined values.
-  Fix optimisation of values used in float arrays (:bugref:`635`).
-  Ensure that combining ``--solver-statistics`` and ``--compiler-statistics`` behaves the same as
   using the ``--statistics`` flag (:bugref:`607`).

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Fix a bug where model selection dialog could run the wrong model.
-  Fix a bug where the same data file could be added to the MiniZinc command twice.
-  Ensure user config directory is created when modifying solver search paths (:idebugref:`167`).
-  Ensure that IDE windows cannot spawn off-screen.
-  Add tooltips to the CP-Profiler status bar legend.
-  Add support for mooc submissions which include file editing history.

.. _v2.6.4:

`Version 2.6.4 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.6.4>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 23 June 2022)

Changes:
^^^^^^^^

-  All considered function items are now checked to ensure that they can be
   correctly evaluated or flattened. This means that, to avoid a type error,
   all functions must have a function body or, in case of a ``var`` function,
   be valid FlatZinc builtins (i.e., they are predicates/return ``var bool``,
   and their arguments are valid FlatZinc types) or, in case of a ``par``
   function, must have an internal definition in the MiniZinc compiler.
   Functions that are defined with ``var`` types for convenient use in output
   can be annotated ``output_only``. The compiler will check that a valid
   ``par`` version of this function is available.

Bug fixes:
^^^^^^^^^^

-  Fix a bug in the type checker where unsupported coercions could cause crashes.
   (:bugref:`581`).
-  Fix a bug in the flattening of binary operators when types of flattened
   arguments match a different version function than matched by the type
   checker. (:bugref:`588`)
-  Fix a bug where statistics without a ``%%%mzn-stat-end`` were not always
   printed in ``--json-stream`` mode.
-  Fix a bug in the task decomposition of the cumulative global constraint where
   resource requirement would not always be correctly be ignored when the task
   duration is zero. (:bugref:`589`)
-  Fix handling of float values in ``.mpc`` parameter configuration files.
-  Fix crash in SCIP plugin due to incorrect loading of ``SCIPinfinity`` symbol.
-  Fix crash in CBC when there is a heuristic solution but no best solution
   (:bugref:`592`).

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^
-  Ensure the extra parameter filter is cleared when the textbox is cleared.

.. _v2.6.3:

`Version 2.6.3 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.6.3>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 6 May 2022)

Changes:
^^^^^^^^

-  Avoid the creation of intermediate ``bool_clause_imp`` calls.
-  Add the ``no_cse`` annotation to allow CSE storage optimization.
-  Improve documentation for ``all_different`` with optional variables.
-  Make breaking of ``defines_var`` cycles deterministic (:bugref:`563`).
-  Improve documentation for warm start search annotations.
-  Automatically detect up to SCIP version 8.0.0 for Windows.
-  Add ``global_cardinality`` and ``global_cardinality_closed`` for arrays of sets.
-  Categorise all global constraints into documentation groups.

Bug fixes:
^^^^^^^^^^

-  Fix a bug where variables required by a solution checker were not always correctly passed to the
   checker, triggering a type error in the checker output.
-  Fix garbage collection bug during typechecking (:bugref:`564`).
-  Fix optimisation of reified ``bool_eq`` constraint where the result must be ``false``
   (:bugref:`565`).
-  Restore missing documentation for functions in the top-level ``globals`` group.
-  Restore ``warm_start`` search annotations for optional values (:bugref:`567`).
-  Fix broken references to top-level library documentation pages.
-  Fix highlighting of JSON-like code blocks in the documentation.
-  Fix a bug where variables used in type-insts of output variables were not correctly copied to the
   output model (:bugref:`570`).
-  Fix a possible crash when flattening ``forall`` calls.
-  Ensure ``pow`` functions are total (:bugref:`572`).
-  Fix an internal error when using an ``if-then-else`` with a branch equating ``<>`` to an
   expression.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Improve UI and dark mode for CP Profiler.
-  Fix CP Profiler tree-builder signal/slot connection (:idebugref:`160`).
-  Fix deadlock in CP Profiler tree building (:idebugref:`162`).
-  Make project loading more resilient to errors (:idebugref:`165`).

.. _v2.6.2:

`Version 2.6.2 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.6.2>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 22 March 2022)

Changes:
^^^^^^^^

-  Better transformation of clauses to avoid negations when possible.

Bug fixes:
^^^^^^^^^^

-  Prevent possible non-deterministic function dispatch.
-  Fix a bug in bounds computation for sums over arrays that could result in a crash.
-  Fix a bug in the logical context of negated implication operators.
-  Fix definition of ``indomain_min`` and ``indomain_max`` for ``set_search`` in ``nosets.mzn``.
-  Fix type checker to consider all functions/predicates with var arguments as var type.
-  Ensure that the last solution is always printed for Gurobi (:bugref:`561`).
-  Fix the dispatch from non-overloaded operators to builtin calls for opt types.
-  Fix creation of ``par`` versions of ``var`` functions used in output.
-  Fix bounds calculation for comprehension ``in`` expressions which are ``var``.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Don't print expected error messages for MOOC submissions.
-  Fix custom parameter widget dark mode CSS.

.. _v2.6.1:

`Version 2.6.1 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.6.1>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 3 March 2022)

Changes:
^^^^^^^^

-  Throw an error when the time limit is reached in ``mzn2fzn`` mode.
-  Relax a MIPD infeasibility check (:bugref:`550`).
-  Add half-reification possibilities for boolean builtin redefinitions.

Bug fixes:
^^^^^^^^^^

-  Fix bounds calculation for singleton linear expression over 0/1 variable.
-  Correctly set whether an array literal is flat during copying (:bugref:`536`, :bugref:`549`).
-  Fix compilation of SCIP and Xpress solver plugins.
-  Don't print the ``=====UNKNOWN=====`` status on timeout in ``mzn2fzn`` mode.
-  Ensure variables are always copied to the output model when required by a solution checker.
-  Rename clashing definition of internal version of ``arrayXd`` to ``arrayNd`` (:bugref:`552`,
   :bugref:`553`).
-  Output the correct variables when using ``--output-mode checker``.
-  Fix possible crash when handling errors due to incorrect garbage collection.
-  Fix WebAssembly build of MiniZinc.
-  Allow comprehensions to iterate over the ``{}`` literal.
-  Fix printing of multidimensional arrays of enums in output.
-  Fix the output format when using ``--json-stream`` on FlatZinc files.
-  Rename ``member_int_reif`` to ``fzn_member_int_reif`` in the Gecode presolver library
   (:bugref:`556`).

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Fix crash when the solver for an unsaved configuration is removed.
-  Fix bug where the selected solver could incorrectly change when a
   configuration option is altered.

.. _v2.6.0:

`Version 2.6.0 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.6.0>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 18 February 2022)

Language and library changes:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  Add support for annotating :mzn:`output` items to define output sections
   which can be switched on/off using ``--only-sections`` and
   ``--not-sections``.
-  Add support for the :mzn:`empty_annotation`, which is removed during
   flattening.
-  Allow annotations on function parameter declarations.
-  Add support for reified annotations. If one of the arguments of an annotation
   declaration is itself annotated with :mzn:`::annotated_expression`, the
   annotation function will be called with the annotated variable at that
   argument position.
-  Support annotations :mzn:`::promise_monotone` and
   :mzn:`::promise_ctx_antitone` for function parameters and let bound
   variables.
-  Add support for capturing annotations in predicates/functions.
-  Add non-optional variants of the :mzn:`in` operator on arrays.
-  Update redefinitions of lex_less(eq) to promote half-reifications.
-  Add definition for bool_clause_imp in standard library.
-  Don't consider quotes as part of identifiers (so :mzn:`'foo'` and :mzn`foo`
   are the same identifier).
-  Add :mzn:`default` operator which takes the value of the right hand side
   if the left hand side is undefined or absent.
-  Remove :mzn:`bool2int` in sum constructs (:bugref:`472`).
-  Rewrite :mzn:`bool_clause_imp` to clause instead of :mzn:`bool_clause`.
-  Remove old, unused solver libraries.
-  Add debugging builtins that are only evaluated with ``--debug``.
-  Add :mzn:`enumOf` function and allow :mzn:`enum_next` and :mzn:`enum_prev`
   with a single parameter.
-  Add support for :mzn:`..<`, :mzn:`<..` and :mzn:`<..<` operators.
-  Add support for open intervals (:mzn:`a..`, :mzn:`..b`, :mzn:`a<..`, etc).
-  Add support for indexed array literals and comprehensions.
-  Allow generators to range over multi-dimensional arrays.
-  Add support for the :mzn:`any` type specifier.
-  Add support for anonymous generators like :mzn:`[ 0 | _ in 1..n ]`.
-  Add support for anonymous enum constructors using :mzn:`_(x)`.
-  Add support for enum construction from a contiguous subset of an enum.
-  Add :mzn:`::output` and :mzn:`::no_output` annotations for top-level and
   local variable declarations.
-  Add support for empty let expressions and trailing comma in enum
   declarations.
-  Add missing variants of :mzn:`=` and :mzn:`!=` operators on arrays by making
   them polymorphic.
-  Change propagation strength annotations from :mzn:`domain` and :mzn:`bounds`
   to :mzn:`domain_propagation` and :mzn:`bounds_propagation` and add
   :mzn:`value_propagation` as an option.
-  Add :mzn:`trace_exp` builtin function for easy tracing of expressions when
   debugging.
-  Add :mzn:`trace_to_section` builtin function.
-  Add :mzn:`relax_and_reconstruct` annotation to standard library.
-  Add ``_decomp`` versions of disjunctive and cumulative optional constraints.
-  Improve :mzn:`sqrt` function.
-  Add IDE visualisation library ``ide/vis.mzn`` along with helpers
   :mzn:`json_object` and :mzn:`json_array` for generating JSON output.
-  Add support for comparing infinities with variables (:bugref:`515`).
-  Weaken bounds to cover more cases in decomposition of
   :mzn:`global_cardinality_low_up`.
-  Allow identifiers starting with underscores in normal MiniZinc models.
-  Make the naming for variants of all_different consistent (:bugref:`500`).
-  Add if-then without else for :mzn:`string`, :mzn:`annotation`, and arrays.
-  Add initial textual structured output functions.
-  Add index poly types for several global constraints and remove top level
   ``_reif`` versions.
-  Add support for hex and octal characters in string literals.

Compiler tool changes:
^^^^^^^^^^^^^^^^^^^^^^

-  Add support for JSON input of enum constructors and anonymous enums.
-  Add ``--cmdline-json-data`` option for passing JSON data on the command line.
-  Add support for JSON stream machine-readable output format enabled with
   ``--json-stream``.
-  Improve generation of default random seed.
-  Use the random seed option for the random builtin functions.
-  Add timeout tracking to the flattening compiler phase.
-  Allow configuring solvers to always pass certain flags or arguments.
-  Honour the ``TMPDIR`` environment variable for placing temporary files
   (:bugref:`468`).
-  Remove temporary files/directories when interrupted (:bugref:`468`).
-  Add ``globals`` section to the output of ``--model-interface-only``.
-  Enable monomorphisation of polymorphic functions.
-  Output one and two-dimensional arrays using index literal syntax when
   possible.
-  Write solution checker warnings directly to the error stream instead of
   showing them as part of the checker output.
-  Make error and warning message output more uniform, improve error messages
   for various errors.
-  Improve reporting of internal errors, asking user to file a bug report.
-  Recognise stack overflows on supported platforms.
-  Add additional message for deprecated functions marked for removal.
-  Don't repeat warnings that are also errors (in particular ResultUndefined
   errors).
-  Add the found and expected array sizes to the error when mismatch occurs
   (:bugref:`510`).
-  Add deprecation warning for type specific usage of overloaded globals.
-  Add warning when included files directly override global constraints.
-  Add delayed rewriting of half-reified bool_clause.
-  Never output :mzn:`_objective` for satisfaction problems.
-  Never include:mzn:`_objective` in model interface output variables.
-  Allow optimisation checkers to take model objective as input.
-  Rewrite :mzn:`array_intersect`, :mzn:`array_union` and :mzn:`array2set`
   functions into predicate calls that can be overridden by solver libraries.
-  Improve error location reporting for type errors involving the objective.
-  Print location and better message when a constraint evaluates to false during
   flattening.

Changes dealing with option types:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  Enable automatic symmetry breaking for absent integer option types by setting
   the internal integer representation to zero (can be disabled with
   ``-Dmzn_absent_zero=false``).
-  Make use of new symmetry breaking in optional :mzn:`min`, :mzn:`max`,
   :mzn:`element` and :mzn:`value_precede`.
-  Add optional versions of :mzn:`circuit`, :mzn:`all_different`,
   :mzn:`inverse`, :mzn:`global_cardinality`, :mzn:`global_cardinality_closed`,
   :mzn:`value_precede_chain`, :mzn:`arg_min`, and :mzn:`arg_max`.
-  Add weak versions of :mzn:`!=`, :mzn:`/` and :mzn:`div` operators.
-  Add weak versions of :mzn:`min` and :mzn:`max` and use these in the
   decomposition of :mzn:`span`.
-  Add :mzn:`::defines_var` for :mzn:`var opt` variables.
-  Add missing :mzn:`opt bool` operators.
-  Add missing :mzn:`in` operators for optional variables.

Changes in MIP solver backends:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
-  Add half-reified alternative for :mzn:`int_ne` in linear library.
-  Add linear definition for :mzn:`bool_clause_imp`.
-  Add support for :mzn:`float_div` in Gurobi and SCIP.
-  Automatically detect Gurobi 9.5.0 and potential future versions.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Add support for specifying submission terms for MOOC.
-  Ensure newly loaded configs override synced options (:idebugref:`144`).
-  Fix check for empty project to avoid incorrect warnings when closing.
-  Maintain modified solver configurations when using preferences dialog.
-  Support using arm64 version of MiniZinc with x86_64 IDE build on macOS.
-  Fix crash when no solver configurations are available.
-  Remove WebEngine-based visualisation feature and implement HTTP/WebSocket
   server based visualisations.
-  Add support for dark mode detection on Windows.
-  Implement foldable output widget supporting output sections.
-  Support both Qt 5 and Qt 6.
-  Allow tab to shift multiple lines right.
-  Re-implement support for detached solver processes.
-  Allow the project/solver configuration panes to take the full height of the
   main window.
-  Implement new multi-tabbed preferences dialog.
-  Ignore errors in non-current files during background code checking.
-  Fix undefined behaviour in main window event filter (:idebugref:`154`).
-  Fix crash when terminating solvers due to closing the main window.
-  Confirm before removing files from project (:idebugref:`149`).

Other changes:
^^^^^^^^^^^^^^

-  Improve table of contents for globals documentation.
-  Restructure the standard library documentation.
-  Add more details on solver installation paths to documentation.
-  Remove useless restart example from documentation.

Bug fixes:
^^^^^^^^^^

-  Fix reference counts for added annotations.
-  Fix cyclic include detection involving multiple model files with the same
   file name (in different directories).
-  Ensure executables have correct extension on Windows (:bugref:`463`).
-  Fix detection of multiple goal hierarchies (:bugref:`465`).
-  Fix :mzn:`trace_stdout` to correctly output to standard output.
-  Fix assertions in graph globals (:bugref:`467`).
-  Fix the decomposition of cumulative (:bugref:`388`).
-  Fix comparison of infinite domains.
-  Pass on fixed partiality status when flattening in root context.
-  Ignore solver exit code when terminating it due to timeout on Windows.
-  Fix the context for constraints in implied_constraint calls (:bugref:`471`).
-  Fix the placement of the bool_clause_imp standard redefinition.
-  Add slicing coercion for let body (:bugref:`483`).
-  Flatten calls on right hand side of variable declarations in root context
   unless they return bool or var bool (:bugref:`485`).
-  Support flattening of top-level variable declarations triggered from non-root
   contexts (:bugref:`485`).
-  Report type error when overloading on return type (:bugref:`484`).
-  Delay deletion of variable, to avoid deleting variable that is required for
   output (:bugref:`476`).
-  Only mark non-toplevel Ids as evaluated if they are not pointing to another
   Id (:bugref:`469`).
-  Report type errors when trying to declare :mzn:`var ann`, :mzn:`var string`,
   :mzn:`var set of bool` or :mzn:`var set of float` (:bugref:`487`).
-  Add missing comparison operators on array (:bugref:`428`).
-  Fix par comparison of sets.
-  Make fix builtin on arrays return array with the same index sets as its
   argument.
-  Don't try to compute bounds for par array literals if they contain var types.
-  Only fail on empty domain if the variable is not a set variable
   (:bugref:`493`).
-  Don't evaluate type-inst variable when creating new flat variable
   (:bugref:`494`).
-  Add missing conditional decomposition for var opt float type (:bugref:`495`).
-  Only extract equalities from if-then-else expressions if they are not on
   arrays.
-  Fix bug that prevented type-checking of type-inst expressions in :mzn:`let`
   variables.
-  Fix cumulative decomposition for fixed resource requirements.
-  Avoid problems with internal annotations in the ``VarOccurrence`` count.
-  Do not pass value from model for :mzn:`var` variable in solution checkers.
-  Correctly handle empty 2D array in :mzn:`show2d` function and empty 3D array
   in :mzn:`show3d` (:bugref:`501`).
-  Fix lost output for functional ``_eq`` constraints (:bugref:`503`).
-  Never insert empty arrays into weak ref tables. (:bugref:`509`).
-  Make sure the new objective variable declaration item is not garbage
   collected before being added to the main model (:bugref:`513`).
-  Fix unification of float variable bounds (:bugref:`514`).
-  Fix :mzn:`deopt` scoping issue in optional :mzn:`min` and :mzn:`max`
   (:bugref:`518`).
-  Create dummy value for option types (:bugref:`521`).
-  Don't rewrite count to :mzn:`count_...` if the counted variables are optional
   (:bugref:`527`).
-  Treat :mzn:`<>` in conditionals as arbitrary type, not int (:bugref:`529`).
-  Fix :mzn:`value_precede_chain` for non 1-based array indexes (:bugref:`530`,
   :bugref:`531`).
-  Ensure the ``Location`` objects in ``Warning`` and ``LocationException`` are
   marked alive (:bugref:`538`).
-  Fix non-reified decomposition of seq_precede_chain for sets.
-  Fixes optional commas and pipes in 2d literals.
-  Ignore :mzn:`<>` in :mzn:`lb_array`, :mzn:`ub_array` and :mzn:`dom_array`.
-  Fix incorrect generation of :mzn:`int_lt` when an optional operand is
   flattened to become non optional.
-  Ensure chain compression considers functional :mzn:`exists` calls in positive
   context.
-  Consider clauses that are not direct implications during chain compression
-  Fix definition of :mzn:`array_intersect`.
-  Fix output of :mzn:`arrayXd` with enum index sets.
-  Fix handling of internal annotations during flattening.
-  Fix JSON output of annotations.
-  Correctly quote DZN IDs in output and correctly escape output variable names
   in model interface.
-  Fix the generation of assertions for enumerated types.
-  Fix computation of function return type with type-inst variable index set and
   enum argument.
-  Move includes from stdlib into solver_redefinitions to ensure any
   corresponding solver-specific predicates get declared in the produced
   latZinc.
-  Fix element functions for arrays of var opt type to work in negative
   contexts.
-  Avoid duplicating expressions in the desugaring of slicing operations.
-  Fix coercion of [] to arbitrary dimensions when used as variable initialiser.
-  Fix calculation of argument type to include actual array argument dimensions.
-  Fix computation of float bounds involving absent value.
-  Fix bind to allow the usage of absent literals in opt float arrays.
-  Always add coercions to variable declarations to make sure array slicing is
   resolved correctly.
-  Make sure the TypeInst of a variable is also scanned for dependencies that
   need to go into the output model.
-  Produce type error for non-Boolean constraint items.
-  Flatten par comprehension generators that contain variables.
-  Flatten boolean array literals and comprehensions in mixed context.
-  Fix :mzn:`var_dom` to correctly handle :mzn:`<>`.
-  Fix segmentation fault in output generation.
-  Ensure that the argument to ``--backend-flags`` does not get consumed early.
-  Fix slice functions to return arrays with enum index set where appropriate.
-  Correctly flatten indexed comprehension with variable :mzn:`where` clause or
   generator.
-  Don't copy comprehensions with variable where clause or generator into output
   model.
-  Restore bindings for lets and calls when an exception is thrown during
   evaluation.
-  Do not output invalid MIP objective statistics for satisfaction problems.
-  Fix flattening of limited partial if-then-else expressions.
-  Fix the rewriting of bool_not reifications when argument is defined.
-  Don't assume all array literals can be evaluated during bounds calculation.
-  Use correct infinite set for floats.
-  Check for undefined results in flat_cv_exp.
-  Don't fail on empty arrays with empty domain (:bugref:`534`).

.. _v2.5.5:

`Version 2.5.5 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.5.5>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 19 March 2021)

Changes:
^^^^^^^^

-  Make min/max on an array of optional variables return a non-optional var,
   behaving as if absent values are not in the array.

Bug fixes:
^^^^^^^^^^

-  Insert par array literals in the common subexpression elimination map, fixing
   a FlatZinc code bloat issue (:bugref:`458`).

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Fix editing of custom string parameters so they don't get converted to
   floats.
-  Fix crash on Windows caused when the ``PATH`` environment contains unicode
   characters.

.. _v2.5.4:

`Version 2.5.4 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.5.4>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 16 March 2021)

Changes:
^^^^^^^^

-  Allow empty arrays in global cardinality constraints (:bugref:`440`).
-  Add piecewise_linear for non-continuous intervals.
-  Fail on empty variable domains in agenda.
-  Allow coercion of JSON lists to enum definitions (:bugref:`441`).
-  Update strictly_decreasing with documentation and opt version (:bugref:`454`).
-  Remove MIP-specific ``fzn_less(eq)_bool(_reif).mzn``.
-  Add ``mzn_in_symmetry_breaking_constraint()`` for internal use.
-  Add MIP decompositions for ``lex_less[eq]_*``.
-  Add ``lex_chain_*`` globals, and use them in ``lex2[_strict]``.
-  Improve detection of variable declarations which are actually par to allow
   more use of overloaded par versions of predicates.
-  Update documentation on installation of OR-Tools.
-  Report CPU time in ``solveTime`` statistic for MIP solvers.

Bug fixes:
^^^^^^^^^^

-  Fix handling of bad Xpress licenses when collecting extra flags.
-  Don't propagate annotations into annotation calls to infinite recursion.
-  Add missing par opt versions of coercion functions.
-  Revert incorrect renaming of ``has_output_item`` to ``has_outputItem`` in
   model interface output.
-  Fix incorrect grammar specification in documentation (:bugref:`453`).
-  Fix crash when defining enums with no members (:bugref:`443`, :bugref:`444`).
-  Support undefined enums in the type checker.
-  Fix CPLEX solver ID in documentation.
-  Never insert par expressions in the common subexpression elimination map.
-  Fix cv flag propagation when the body of a let or function is cv.
-  Fix equality test for annotations involving indirection.
-  Don't modify the infinite domain of optional variables (:bugref:`456`).
-  Don't immediately evaluate output_only arrays when generating dzn output.
-  Coerce boolean objectives to integers.
-  Don't create copies of global declarations when creating par versions of
   functions.
-  Compile infinite domains with holes into constraints (:bugref:`457`).
-  Use generic flattening inside generators, disallowing free boolean variables
   inside ``in`` expressions (:bugref:`451`).
-  Strip library paths from includes in multi-pass compilation (:bugref:`455`).
-  Canonicalise file names of includes to ensure the same file is not included
   multiple times.
-  Escape paths in printed ``include`` items, fixing backslash problems on
   Windows.
-  Follow ids to declarations when flattening par arrays (:bugref:`448`).
-  Ignore par constants during chain compression.
-  Fix flattening of all-par set literals.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Fix possible crash due to incorrect use of WriteFile on Windows.
-  Ensure Gecode Gist dependencies are present in the Linux bundle and AppImage
   (:idebugref:`132`).
-  Fix crash when stopping solver during exit.
-  Don't show irrelevant context menu entries in the project explorer.
-  Add support for HTTP/S links in the output pane.
-  Fix crash when saving CP Profiler executions where there is no info
   associated with a node.
-  Show a warning when there are open files which are not part of a MOOC
   submission.
-  Fix double spinbox precision issues (:idebugref:`134`).
-  Include Gecode Gist and CP Profiler dependencies in Snap package.
-  Allow opening of multiple files through the open file menu option.
-  Ensure file dialogs save last path when opening files.
-  Make the escape key close the find/replace dialog when focussed on any child
   widget.
-  Allow setting MOOC submission items as mandatory.

.. _v2.5.3:

`Version 2.5.3 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.5.3>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 24 November 2020)

Changes:
^^^^^^^^

-  Fully reify -> (x != y) in the linear library.
-  Allow printing of comprehensions using introduced variables.
-  Allow increasing/decreasing over multidimensional arrays.
-  Add mzn_ignore_symmetry_breaking_constraints and mzn_ignore_redundant_constraints
   options, allowing the symmetry_breaking_constraint and redundant_constraint
   predicates to be overridden, so that those constraints can be disabled independent
   of the solver library that's being used (:bugref:`429`).
-  Add automatic coercion of strings in JSON input data to enum constants where needed.
-  Add automatic coercion of lists in JSON input data to sets where needed.

Bug fixes:
^^^^^^^^^^

-  Fix int_lin_eq_imp in the linear library.
-  Use variable declaration location for invalid type-inst error messages without
   locations.
-  Rewrite par versions of fzn_count_* into var versions, allowing solvers that
   only redefine the bar version to use their built-in propagators even if the
   value to count is fixed at compile time (:bugref:`427`).
-  Add multi-level array construction for enumerated types when outputting in
   JSON format.
-  Ensure that functions can only be used as par if their return type is par
   (:bugref:`431`).
-  Fix parser default location macro, preventing loss of location filenames
   in some cases.
-  Fix parser rule for non-opt sets to give the correct starting location.
-  Fix fzn_bin_packing_capa_reif.mzn and fzn_bin_packing_load_reif.mzn
   (:bugref:`435`).
-  Update decl for binary and unary operators when creating par versions of
   functions (:bugref:`437`).
-  Only throw type errors for enum type identifier mismatch in strict enums mode.
-  Only post cumulative constraints if there is at least one task, preventing an
   assertion about the lower bound from failing.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Only reset config window item focus if it is still focused, preventing spurious
   changes in focus during code checking.
-  Fix handling of final statuses, including UNSAT (:idebugref:`123`).
-  Remove -s flag support from Gecode Gist solver configuration (:idebugref:`125`).
-  Fix crash when saving a project with no solver selected (:idebugref:`127`).
-  Correctly remove temporary parameter configuration files after use
   (:idebugref:`128`, :idebugref:`129`).
-  Fix the time limit readout in the status bar when solving.

.. _v2.5.2:

`Version 2.5.2 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.5.2>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 6 November 2020)

Changes:
^^^^^^^^

-  Use full reification in int_ne_imp.
-  Add support for redefining 2d element constraints in the solver library.
-  Produce warning when shadowing a variable in a let or comprehension in the
   same function (or toplevel) scope (:bugref:`419`).
-  Rewrite symmetric_all_different to inverse (:bugref:`426`).
-  Add link icons to globals etc in the reference documentation (:bugref:`425`).
-  Make the nodes statistic show the total number of nodes across all restarts
   for SCIP.
-  Add support for multidimensional arrays in counting constraints (:bugref:`413`).
-  Allow .json files to be specified using the --data option (in addition to
   .dzn files).
-  When specifying relative paths inside parameter configuration files,
   resolve them relative to the config file.

Bug fixes:
^^^^^^^^^^

-  Correctly add file extension to plugin libraries when omitted.
-  Fix JSON array index coercion when the first index is undefined.
-  Catch ResultUndefined exception when evaluating cv par expressions,
   and turn into undefined result.
-  Fix trailing for lets and comprehensions, resolving some issues with
   recursive functions containing lets and/or comprehensions.
-  Only create par version of functions that do not refer to any toplevel
   variables (:bugref:`418`).
-  Keep correct location information for identifiers.
-  Print warnings from solns2out.
-  Fix the removal of reverse mapped arrays when they contain aliases.
-  Disallow macro replacement when call has reification implementation.
-  Fix the behaviour of passing an invalid version hint to --solver.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Properly resize extra flags table after adding parameters (:idebugref:`119`).
-  Use the minimal configuration to check the model interface
   (:idebugref:`118`).
-  Allow omitting builtin solver version in project JSON.
-  Don't mark as modified when loading non-synced solver configurations.
-  Ensure the last open configuration in a project is selected when loaded.
-  Fix the default values of solution truncation and output window clearing.
-  Process unrecognised extra flags from old project configurations.
-  Fix watching for modification of the additional data box.
-  Fix the alignment of line numbers.
-  Make behaviour controls more narrow to accommodate smaller window sizes.
-  Defocus config window widgets when updating solver config so values of
   currently edited fields are updated.
-  Pass user input data correctly during compilation.
-  Remove solns2out options from MiniZinc call when compiling.

.. _v2.5.1:

`Version 2.5.1 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.5.1>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 22 October 2020)

Changes:
^^^^^^^^

-  Rewrite alldifferent_except_0 to fzn_alldifferent_except_0, to enable
   solvers to implement that constraint if it is available (:bugref:`414`).
-  Propagate domains for variables even when reverse-mapped. This
   ensures that variables with multiple encodings can be created with
   the tightest possible bounds.
-  Fail instead of producing empty domains when simplifying int_le
   constraints.
-  Allow parsing of nested objects in parameter configuration files.
-  Add --backend-flags option to provide a uniform way of passing flags
   to an underlying solver.
-  Add extra flags support to the MIP solver interfaces, allowing
   parameters to be set in the IDE.
-  Improve automatic detection of the Xpress solver and license file.
-  Allow the use of spaces in the --solver flag argument.
-  Automatically add the last part of the solver ID as a tag.
-  Improve handling of var functions in output, automatically creating
   par versions of var functions if possible.

Bug fixes:
^^^^^^^^^^

-  Fix parsing of empty multidimensional JSON arrays.
-  Allow use of --parallel long form option in MIP solvers.
-  Fix item lookup when increasing annotation usage in annotate builtin.
-  Fix JSON array coercion to handle arrays with 1 unknown index.
-  Don't try to access array dimensions for output of empty
   multi-dimensional arrays.
-  Print verbose version information to stderr instead of stdout.
-  Fix context handling when flattening par expressions that contain
   variables (:bugref:`415`).
-  Flatten string expressions if they contain variable parts in
   assert/abort/trace calls.
-  Fix breakage on older versions of Windows due to UTF-8 conversion
   failing.
-  Remove defines_var/is_defined_var annotations when simplifying
   boolean constraints.
-  Fix transfer of cv status from where parts to newly generated
   conjunctions during typechecking.
-  Fix multiple issues with the defined_var / is_defined_var
   annotations.
-  Move all included files from stdlib into solver_redefinitions.mzn, so
   that solver redefinitions are not marked as belonging to the standard
   library (:bugref:`416`).
-  Fix documentation group for standard annotations (:bugref:`417`).
-  Show correct version of solver plugins which have their DLLs
   specified using a command-line parameter (:bugref:`411`).
-  Fix arbitrary flag support for NL solvers.
-  Kill child processes if exception occurs during solns2out on
   Unix-like platforms.

Changes in the IDE:
^^^^^^^^^^^^^^^^^^^

-  Fix typo when passing solver statistics option to minizinc (:idebugref:`112`).
-  Fix missing statistics output (:idebugref:`112`).
-  Add support for colour themes (:idebugref:`110`).
-  Don't prompt for saving after adding/removing files from the Untitled
   project.
-  Fix running of compiled FlatZinc files.
-  Show error message when trying to load an invalid configuration file.
-  Ensure all output is sent to the output console, and that fragments
   in standard error output appear when a newline is written to standard
   output (:idebugref:`114`).
-  Fix running of solver configurations from the project explorer.
-  Improve performance of adding a large number of extra flags at once.
-  Add support for 64-bit integer extra flags.
-  Add support for setting both solver backend flags and MiniZinc
   command flags (:idebugref:`113`).
-  Improve interface for adding extra parameters, allowing search/filter
   and multiselection of known parameters.

.. _v2.5.0:

`Version 2.5.0 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.5.0>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 6 October 2020)

Language, tool and library changes:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  Allow `reading command line arguments from JSON config
   file </doc-2.5.0/en/command_line.html#ch-param-files>`__.
-  Add support for `enum
   constructors </doc-2.5.0/en/modelling2.html#enumerated-types>`__.
-  Put subprocesses in their own process group so that they don't
   receive signals from both the console and MiniZinc.
-  Implement soft and hard process timeouts on Windows, allow triggering
   of shutdown from named pipe on Windows for the IDE.
-  Make MiniZinc unicode-aware on Windows.
-  Better error messages for index set mismatches.
-  Report similar identifiers when matching fails.
-  Better error messages when a call cannot be matched to an existing
   function or predicate.
-  Print error stack if top of stack is a single identifier (i.e., error
   occurred while flattening a variable declaration).
-  Add new separate flags for intermediate and all solutions. -i enables
   intermediate solutions for optimisation problems and
   --all-satisfaction enables all solutions for satisfaction problems.

Changes in interfaces to solvers:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  Solvers which only support intermediate solutions now can now support
   the standard flag -i rather than -a.
-  Restructure the `MiniZinc standard
   library </doc-2.5.0/en/lib-stdlib.html#standard-library>`__.

Changes in MIP solver backends:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  Remove non-conforming -n flags for MIP solver configs standard flags.
-  Improve autodetection of Gurobi DLL.
-  Find Gurobi 9.0.2 when building.
-  Don't create gurobi log.
-  Interface to concurrent solves in Gurobi (--readConcurrentParam).
-  Add -DMinMaxGeneral option for min/max as fzn_array_float_minimum for
   Gurobi
-  Find SCIP 7.0 on Windows
-  Use -Glinear library, built-in cumulative by default for SCIP.
-  Use quadratics in Gurobi and SCIP by default.
-  Add options --xpress-root and --xpress-password for finding Xpress
   installation directory and licence file.
-  Add MIQCP quadratic constraints for Gurobi and SCIP.

Changes dealing with option types:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  Add opt versions of several globals.
-  Define weak equality for var opt bool.
-  Add set_in definitions for var opt int.
-  Add opt versions of enumerated type functions (to_enum, enum_next,
   enum_prev etc).
-  Enable set literals with optional values (which will be ignored),
   including var set literals with var opt int elements.
-  Add opt version of float_dom to stdlib.
-  Change unary not for opt bool to use absorption lifting.
-  Add array2set functions on var opt int arrays.
-  Add opt versions of dom, dom_array and dom_bounds_array.
-  Add missing logical operators to var opt bool.

Changes in the MiniZinc IDE:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  Remove support for the old binary storage format of projects. These
   must be opened and re-saved with version 2.4.3 to remain compatible.
-  Include experimental CP-profiler through the \*MiniZinc\* > \*Profile
   search\* option for supported solvers.
-  Redesign the solver configuration window.
-  Use parameter configuration files rather than passing command-line
   options directly.
-  Show solver configurations and checkers in their own sections in the
   project explorer.
-  Allow multiselection in the project explorer for running particular
   sets of files.
-  Allow MiniZinc to manage subprocesses by itself.
-  Allow non-privileged installs of the IDE on Windows.
-  Correctly remove files from old installations of the IDE on Windows.
-  Enable scroll bars in the preferences dialog to allow for low
   resolution displays.
-  Prompt to save modified files before performing MOOC submissions or
   running models.
-  Fix infinite recursion when a model file for a MOOC submission
   doesn't exist.
-  Use --output-mode checker for MOOC solution submission where
   supported.
-  Fully support unicode on Windows.

Minor changes:
^^^^^^^^^^^^^^

-  Clean up code base and format using clang-format and clang-tidy.
-  Update WebAssembly build for new versions of emscripten.
-  Support --cp-profiler option to activate search profiler for IDE.
-  Add --solver-json to output single solver config as JSON.
-  Coerce JSON arrays to match the MiniZinc TypeInst.
-  Add more informative README file.
-  Split shared MIP cpp code into seperate CMake object target.
-  Compile with POSITION_INDEPENDENT_CODE targets by default.
-  Change ASTString to use String Interning.
-  Add included_files output to model interface.
-  Update Bison parsers to be compatible with Bison 3.7.
-  Allow annotating enum declarations.
-  Add support for --c_d and --a_d options to set recomputation
   commit/adaption distance for Gecode presolver.
-  Place float_set_in in a version redefinition documentation group.
-  Place int_pow_fixed into a version redefinitions group.
-  Move set_in(var int, set of int) to the Integer FlatZinc Builtins.
-  Make "show" display contents of arrays rather than array identifiers
   if the array is var
-  Add support for checking statistics after solving has finished.
-  Include preferences set by IDE repository.
-  Add has_ann(var, ann) annotation introspection builtin.
-  Use reverse mapped version for output if FlatZinc contains an aliased
   variable.
-  Remove NDEBUG flag from the compile flags added by CPLEX and Gurobi.
-  Use integer variables in decomposition for array_int_element,
   array_var_int_element, array_int_minimum, and array_int_maximum.
-  More preprocessing for pow(int, int).
-  Add is_same builtin.
-  Add multiobjective annotation for Gurobi and Yuck (in
   std/experimental.mzn).
-  Add --output-mode checker, which outputs exactly the variables that
   are required for a given solution checker.
-  Improve propagation of annotations, especially for redefined forall,
   exists, clause, xor
-  Make omitting RHS from output_only variable a type error.
-  Add support for var set comprehensions
-  Make sets inside set literals a type error (rather than evaluation
   error).
-  Aggregate bool_not into exists/clause, use bool_not(e) for
   clause([],[e]) expressions
-  Cleanup the common-subexpression elimination table.
-  Generate bool_not calls (instead of bool_eq_reif) and add both "x=not
   y" and "y=not x" into the CSE map, to avoid double negations.
-  Add arg_max and arg_min builtins for boolean arrays.
-  Remove -O flag from ozn file naming.
-  Allow var items in checkers to be omitted from the model.
-  Add builtins for binary operators that have a var redefinition.
-  When an integer or bool variable has a singleton domain, use the
   value. This enables more overloading to par functions.
-  Check if domain becomes empty when binding variable to value,
   avoiding empty domains (such as 1..0) in FlatZinc.
-  Ignore unknown JSON data items instead of throwing an error.
-  Add trace_logstream and logstream_to_string builtins. These can be
   used for writing model checkers/graders, but also for general
   logging.
-  Clean up CMake configuration
-  Allow any installed solver to be used with the test suite, add
   ability to test for expected ozn output.

.. _bug-fixes-1:

Bug fixes:
^^^^^^^^^^

-  Fix error message for type errors in domains that are integer
   literals (:bugref:`408`).
-  Fix comprehensions over option types, which could cause crashes and
   incorrect flattening (:bugref:`407`).
-  Fix the usage count of annotations added using the annotate function
-  Flatten "in" expressions in comprehensions when required.
-  Check if operator is built-in after evaluating arguments, to make
   sure it is rewritten into the correct predicate.
-  Use dom(x) instead of lb(x)..ub(x) for opt int.
-  Use eval_par to compute bounds for par expressions since they might
   be opt.
-  Use library defined operators where available.
-  Fix -O flag parsing for optimisation level.
-  Fix par set inequality calculation.
-  Flatten domain expressions that contain variables.
-  Catch ResultUndefined when flattening an array with an undefined
   expression in a generator
-  Fix source paths in MD5 generation scripts.
-  Fix crash when reporting undefined result in assignment generator.
-  Only add coercion for normal generators, not for assignment
   generators.
-  Check output var status on actual item being removed.
-  Include absolute path instead of filename in multipass processing.
-  Coerce comprehension generators if necessary, so that slicing
   notation can be used there.
-  Fix copying of comprehensions with followIds.
-  Fix the method signature of printStatistics for Geas.
-  Ensure the definition of reverse mappers are copied into the output
   model.
-  Print solns2out statistics to stdout to comply with MiniZinc spec.
-  Minor doc-fix for global_cardinality_closed.
-  Make statistics output comply with MiniZinc spec.
-  Fix reverse function to work with empty arrays
-  Fix the coercion of boolean sum in aggregation.
-  Remove eval_par on var expressions in show builtin.
-  Fix the table construction for the Geas solver interface
-  Fixed wrong sign in Boolean linear constraints in Geas solver
   interface.
-  Fix istrue and isfalse by using flat_cv_exp if necessary.
-  Fix the excess flattening of items marked for removal.
-  Do not add newline to output when killing FlatZinc solver process,
   since this may be in the middle of a line
-  Fix typo in loop for Geas solver instance.
-  Don't call doAddVars when there are no variables, fixing a crash in
   MIP solvers for empty models.
-  Do not copy type of lhs onto rhs when parsing solutions. This tagged
   some literals as cv(), which broke the evaluation.
-  Fix flattening of all par set literals.
-  Fix error macro to be compatible with newer versions of Bison (:bugref:`389`).
-  Fix printing of if-then-else expressions without an else branch.
-  Fix allowed solvers option in test suite.
-  Make bind only create an int_eq constraint if a variable has a
   reverse mapper.
-  Fix automatic coercions to keep cv type attribute of their argument
   (:bugref:`387`).
-  Fix copying of output_only variables to the output model.
-  Only print checker output for unique solutions.
-  Fix rewriting of lin_exp into int/float_lin_eq.
-  Fix flattening of calls and let expressions that have par type but
   contain var expressions.
-  Use eval_bool instead of eval_par for boolean evaluation.
-  Remove the direct assignment to a domain if it has a reverse mapper.
-  Fix arg_max and arg_min for array index sets not starting at 1.
-  Add missing set_superset_reif FlatZinc predicate.
-  Fix counting of non-fixed variables in Boolean constraints. Could
   previously lead to incorrect simplifications.
-  Enable eval_floatset for SetLits that contain an IntSetVal. This is
   used during chain compression and could previously result in
   incorrect domains.
-  Fix bugs in chain compressor caused by modifying multimaps while
   iterating over them.
-  Fix crash when cleaning up after running builtin Gecode.
-  MIPdomains: don't assume equations have no literals.
-  Only fix domain after flattening bool_eq.
-  Only return singleton domain as value for non-optional variables.
-  When evaluating identifier that is bound to a fixed value, check that
   the value is inside the domain to correctly detect model
   inconsistency.
-  Add missing assert and trace builtin overloads.
-  Flatten expressions that may contain variables in par where clauses.
-  Fix segmentation fault when the declaration of an array is passed to
   setComputedDomains with the -g parameter.
-  Consider single-valued domain variables to be fixed
-  Add missing definition of to_enum for arrays of sets.
-  Evaluate partiality of arguments even if call was already in CSE
   table (:bugref:`374`).

.. _v2.4.3:

`Version 2.4.3 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.4.3>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 4 March 2020)

.. _changes-1:

Changes:
^^^^^^^^

-  Enable CPLEX 12.10.
-  Add checker output to generated output items.
-  Short-circuit evaluation for mixed par/var conjunctions,
   disjunctions, and clauses.
-  Add inverse_in_range global.
-  Pretty printing set ranges now uses union instead of ++ to be
   compatible with DZN.
-  Add array2set for par bool and float arrays
-  The \_objective variable is no longer added to FlatZinc input files.
-  JSON representation of sets with ranges can now be parsed (previously
   they could only be output).
-  Check index sets to arguments of global_cardinality_low_up.
-  Xpress and SCIP are not compiled as plugins and no longer require
   recompilation to enable.
-  If-then-else for opt are no longer written in terms of the non-opt
   version, allowing them to return absent.

.. _bug-fixes-2:

Bug fixes:
^^^^^^^^^^

-  Fix checking of domains and index sets in par arrays inside lets.
-  Remove duplicate call stack items to improve error messages.
-  Ignore absent values when computing domains.
-  Generate call for actual binary operator (after optimising double
   negation). Fixes :bugref:`364`.
-  Fix non-associative operators on optional values.
-  Only output optional parameters in model interface if they were
   undefined (rather than assigned to <>).
-  Fix some issues with evaluating par opt expressions.
-  Make solution checkers work for multi-dimensional arrays and arrays
   with enum index sets
-  Fix Boolean aggregation for expressions that are defined recursively.
-  Use correct index set for nosets set_lt and similar (partial fix for
   :bugref:`369`)
-  Fix coercion of sets to arrays (previously, coercing a set of X to an
   array of X to an array of Y did not work correctly).
-  Fix infinite loop when printing infinite set range
-  Add assertion so that array2set can only be used for arrays with
   bounds (:bugref:`370`, :bugref:`371`).
-  Fix typing and pretty printing of par bool sets.
-  Use output_array dims for output vars in FlatZinc files (previously,
   a type-checker error would occur when running a solver through
   MiniZinc on a FlatZinc file with multidimensional arrays).
-  The Xpress backend was made functional again.
-  Fix segmentation fault in output_only type-checking.
-  Compute correct array enum type for array slices (:bugref:`372`).
-  Fix behaviour of using undefined expressions in var comprehensions
   guarded against by where clauses (previously, these undefined
   expressions would bubble up regardless of the where clause,
   constraining the model).
-  IDE: Disable menu items that don't make sense when all tabs are
   closed, fix behaviour of stop button when all tabs closed (fixes
   several crashes).
-  IDE: Add x86_64 suffix to linux package name (:idebugref:`96`).
-  IDE: Make boolean extra solver options with a default of true
   functional.
-  IDE: Only read linter results if it exited normally (:idebugref:`97`).
-  IDE: Resolve paths in \_mooc to paths (allowing submission of models
   in subdirectories).

.. _v2.4.2:

`Version 2.4.2 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.4.2>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 10 January 2020)

.. _changes-2:

Changes:
^^^^^^^^

-  The test suite is now integrated into the continuous integration
   system.

.. _bug-fixes-3:

Bug fixes:
^^^^^^^^^^

-  Fix flattening of negated disjunctions (:bugref:`359`).
-  Fix simplification of Boolean constraints (repeated simplification
   could sometimes crash).
-  Fix memory management during flattening of conditionals (:bugref:`358`).
-  Fix type inference for rewriting of sums into count constraints, and
   only apply the rewriting for var type-insts.
-  Fix handling of solution checkers (these used to produce spurious
   error messages).
-  IDE: Fix syntax highlighting of keywords, and add syntax highlighting
   for interpolated strings.
-  IDE: Redraw when switching to/from dark mode, and fix dark mode
   header colours.
-  IDE: Fix "Select all" menu item.

.. _v2.4.1:

`Version 2.4.1 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.4.1>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 20 December 2019)

.. _changes-3:

Changes:
^^^^^^^^

-  Improve compiler optimisation for some linear, multiplication and
   Boolean constraints.
-  Improved translation of lex and all_equal constraints when the arrays
   have no or only one variable.
-  IDE: Display error message when submission to MOOC provider fails.
-  IDE: Make "previous tab" and "next tab" actions cycle rather than
   stop at first/last tab.

.. _bug-fixes-4:

Bug fixes:
^^^^^^^^^^

-  Fixed regular expression constraint for expressions containing
   negated character classes (^ operator).
-  Fix element constraint in nosets.mzn library when set domains are not
   contiguous.
-  Correctly identify Windows paths starting with // or \\\\ as absolute
   (this enables the parser to open files stored on network drives).
-  Use set_in constraints (rather than int_in) for internal Gecode-based
   presolver. This fixes some issues when compiling with -O3.
-  The optimisation phase of the compiler now fully substitutes par bool
   variables (these can be introduced into the FlatZinc during multipass
   compilation). (:bugref:`357`)
-  Fixed the reference counting for variables that are re-used in
   multipass compilation. (:bugref:`357`)
-  Remove incorrect error handling when parsing from strings rather than
   files. Partially fixes (:bugref:`357`)
-  Made the is_fixed builtin work for more types. (:bugref:`356`)
-  Enable rewriting of sum(i in x)(i=c) op d and count(x,y) op z into
   global counting constraints.
-  Split up count global constraints into separate files for reified
   versions.
-  Use contiguous range for array index set in set_lt for nosets.mzn.
-  Negate results of conditionals if required. (:bugref:`355`)
-  Partiality of conditional needs to be translated in root context
   (even if conditional itself is negated). (:bugref:`355`)
-  Don't copy function into output again if it was already copied (and
   made par) before. (:bugref:`323`)
-  Define card function on var sets in terms of set_card FlatZinc
   builtin.
-  Don't set bounds for set variables in internal Gecode presolver.
-  IDE: Fix shift left and shift right indentation behaviour when
   selecting text backwards.
-  IDE: Fix OpenSSL library in binary distribution to enable update
   checks and submission to MOOCs again.

.. _v2.4.0:

`Version 2.4.0 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.4.0>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 13 December 2019)

.. _changes-4:

Changes:
^^^^^^^^

-  The compiler now detects counting constraints in expressions such as
   count(i in x)(i=3) <= 4 and rewrites them into global counting
   constraints. This is now the preferred way to specify counting. The
   atmost/atleast/exactly constraints on integer variables have been
   deprecated, and versions of count predicates with par variables have
   been added. FlatZinc solvers that supported atmost/atleast/exactly
   should now support the corresponding fzn_count\_?_par predicates.
-  The compiler now supports the command line option
   --output-detailed-timing, which provides timing information for each
   toplevel constraint item, or for each line of code when used in
   conjunction with the --keep-paths option.
-  The library now contains annotations for deprecated library
   functions.
-  A par version of the inverse function has been added (include
   inverse_fn.mzn to use it).
-  The common case of sums of optional variables is now handled more
   efficiently. This case often arises from generator expressions with
   variable where clauses.
-  Added set_to_ranges built-ins to enable efficient iteration over
   sets. These are used to implement set_in for float variables, which
   was missing before.
-  The Gurobi and CPLEX backends now support the --random-seed command
   line option.
-  The Gurobi and CPLEX backends now use nodefile for search trees
   exceeding 500 MB (--nodefilestart can change this value and
   --nodefiledir the folder.)
-  The MIPDomains optimisations have been switched back on by default.
   The optimisations have also been strengthened for some special cases.
-  Without the MIPdomains postprocessing, linearisation of variable
   domains with holes now uses set_in instead of individual not-equal
   constraints, which may result in more compact FlatZinc.
-  Linearisation of multiplication can now consider the exact domain of
   a factor.
-  The product functions have been made non-recursive in order to
   support longer arrays.
-  Bounds inference for results of if-then-else expressions has been
   improved.
-  Support for optional float variables has been added.
-  The interfaces to CBC, CPLEX and Gurobi now report correctly that
   they support verbose output during solving (so that the "verbose
   solving" option is available from the MiniZinc IDE).
-  IDE: Parse timing and statistics output produced by compiler, and
   display as profiling information next to each line in the model.
-  IDE: Enable run/compile action on data files. This automatically
   selects the model file if there is only one, or presents a dialog for
   selecting the model if there are multiple.
-  IDE: Select first data file in parameter dialog if there was no
   previous selection, and always focus parameter dialog.
-  IDE: Highlight current line.
-  IDE: Support .json as file extension for data files.
-  IDE: Remember whether wrap around, case sensitivity and regular
   expression was selected in find/replace dialog, pre-select the
   find/replace text when find/replace widget is openend, and close
   find/replace widget when ESC is pressed while editor has focus.

.. _bug-fixes-5:

Bug fixes:
^^^^^^^^^^

-  Fixed output handling on Windows (output is now processed on the main
   thread, so that exceptions thrown during output are printed
   correctly, and memory management is thread safe).
-  Fixed decomposition of reified mdd constraint, and strengthened
   decompositions of mdd and cost_mdd.
-  Fix handling of variable re-definitions (e.g. converting sets to
   arrays of bools), which would previously sometimes result in
   variables being removed although they were required for output, or
   the reverse mapping function not being available in the output model.
-  Include regular.mzn from regular_regexp.mzn. (:bugref:`351`)
-  Inlining of function calls has been moved from the flattener into the
   type checker, and it now is more strict about which functions can be
   inlined in order to avoid overloading issues.
-  Updated fzn_count_{neq,leq,lt,geq,gt},
   fzn_global_cardinality_low_up{,_reif} to use use the count_eq
   predicate. (:bugref:`334`, :bugref:`335`)
-  Fixed the documentation for several constraints, which did not
   display as bullet point lists as intended.
-  Copy function/predicate declarations into FlatZinc without
   annotations, since most FlatZinc parsers would not expect annotations
   and fail to parse.
-  Process right hand side of par VarDecls to make sure any identifiers
   it uses are copied into the output model. Fixes :bugref:`336`.
-  Fix type checking for conditionals where the else branch has enum
   type but the then branch has int type.
-  Make the deopt function return correct enum instead of int type.
-  Fix for path handling when 'needRangeDomains' is active. Avoids
   infinite recursion in the compiler.
-  Fix race condition in temporary file generator for Windows. (:bugref:`349`)
-  Register fzn\_ names for Gecode presolver. Fixes command line flags
   -O3 and above.
-  Fix par evaluation of float and bool set comprehensions.
-  Fix documentation of array_bool_xor. Fixes :docbugref:`13`.
-  Fix the round() built-in to correctly round negative numbers
-  Fix computation of intersection of domains when assigning an array to
   an array variable. Fixes :bugref:`310`.
-  Add defines_var annotations for functional global constraints. Fixes
   :bugref:`345`.
-  Add set_lt_reif/set_le_reif to flatzinc builtins library. Fixes :bugref:`338`.
-  Clarify set order based on spec. Fixes :bugref:`339`.
-  Don't return already removed VarDecl objects from CSE. Fixes :bugref:`346`.
-  Do not post y!=0 constraint if 0 is not in the domain (workaround for
   a limitation in the handling of basic float constraints). Fixes :bugref:`344`.
-  Help type checker by making deopt/occurs argument types explicit.
   Fixes :bugref:`331`.
-  Fix transfer of domains when aliasing one variable to another
-  MIP: fix for aux_float_ne_if_1
-  MIP: int_(eq/ne)_imp: don't force eq_encode without MIPdomains
-  Fix a typo in the definition of fzn_at_least_int{,_reif}
-  Fix dependency problem in the gecode_presolver table specification
-  Add seq_precede_chain.mzn to globals.mzn. Fixes :bugref:`332`.
-  Don't assign right hand side of set variables if domain is singleton.
   Fixes :bugref:`330`.
-  Don't invalidate float bound just because an expression contains an
   integer.
-  Fix copying of let expressions.
-  Put lexer and parser helper functions into MiniZinc namespace to
   avoid linker issues. Fixes :bugref:`325`.
-  Reset array index sets defined in lets inside recursive function
   calls.
-  Arrays copied into output model need to have par type-inst. Fixes :bugref:`322`.
-  Don't complain when same function is registered twice. Fixes :bugref:`323`.
-  Fix partiality handling of if-then-else expressions.
-  Track whether variable is used in an output array before making
   decision to compress implication chains. Fixes :bugref:`318`.
-  IDE: Fix dark mode detection on macOS 10.15, improve dark mode colors
   a bit and fixed some dark mode bugs.
-  IDE: Make background compilation of a model (used to display syntax
   and type errors) a bit more stable.
-  IDE: Avoid infinite loop in wrap around replace all.
-  IDE: Fix memory management for HTML visualisation windows, and resize
   docked HTML visualisation widgets to take up equal space.

.. _v2.3.2:

`Version 2.3.2 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.3.2>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 12 September 2019)

.. _changes-5:

Changes:
^^^^^^^^

-  Add warm starts and subtour cuts to CBC interface.
-  Add documentation and assertion requiring that mdds are
   deterministic, and add nondeterministic variant of mdd constraint.
-  Add -s to the standard flags supported by MIP interfaces.
-  Add flag --output-output-item to include user specified output item
   in the formatted JSON and DZN output.

.. _bug-fixes-6:

Bug fixes:
^^^^^^^^^^

-  Fix a bug that could leave unused variables in the resulting
   FlatZinc.
-  bounded_dpath should rewrite to fzn_bounded_dpath. Fixes :bugref:`300`.
-  Fix definition of sum_set.
-  Check if overloaded function required for output. Fixes :bugref:`303`.
-  Move regular constraint with set argument to its own file.
-  Flatten assignment generators if necessary.
-  Simplify fzn_value_precede_chain_int and avoid use of element
   predicate. Fixes :bugref:`307`.
-  Only initialise par opt variables as absent if they are not arrays.
-  Fix the description of the neural_net predicate.
-  Fix regular constraint with regular expressions (stopped working in
   2.3.0).
-  Fix the model interface output to include the same variables as the
   generated output statement.
-  Fix CSE for removed variable declarations. Could lead to reified
   constraints not being compiled correctly when the control variable
   got fixed to true.

.. _v2.3.1:

`Version 2.3.1 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.3.1>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 10 July 2019)

.. _bug-fixes-7:

Bug fixes:
^^^^^^^^^^

-  Report error when trying to assign an array literal to an array
   variable with incompatible index set.
-  Fix partial evaluation of expressions, so that only par expressions
   are fully evaluated. Fixes :bugref:`298`.
-  Remove carriage returns when reading piped solver output on Windows.
-  Canonicalize paths of executables to avoid spurious warnings about
   multiple executables for the same solver.
-  Add implementations for != on arrays.
-  Compute quotient bounds before decomposition of int_div in
   linearisation library.
-  Propagate domain constraints on variables that are aliased
   (previously domain constraints could get lost).
-  Propagate domain constraints from left-hand-side to right-hand-side
   in variable assignments.
-  piecewise-linear: reuse decomposition for X when only Y-values
   change.
-  nosets: add set_in_imp(var set) and simplify set_in_reif, set_eq(var
   set, var set).
-  linearisation: improved compilation of set_in constraints.
-  MiniZinc IDE: Remove incorrect symbolic link and fix qt.conf for some
   bundled distributions.
-  MiniZinc IDE: Fix check for availability of dark mode on older
   versions of macOS.
-  MiniZinc IDE: Fix a typo in the cheat sheet.
-  MiniZinc IDE: Provide more robust solution for checking the model
   parameters, which will get rid of some "internal error" messages.
-  MiniZinc IDE: Always show directory selection dialog in the Windows
   installer. Addresses :idebugref:`89`.
-  MiniZinc IDE: Improved the configuration files for some bundled
   solvers, provides nicer configuration interface.

.. _v2.3.0:

`Version 2.3.0 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.3.0>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 26 June 2019)

Major changes:
^^^^^^^^^^^^^^

-  The compiler can now generate FlatZinc with half reified constraints.
   See
   https://www.minizinc.org/doc-2.3.0/en/fzn-spec.html#reified-and-half-reified-predicates
   for more details.
-  The standard library of global constraints has been reorganised,
   making it easier for solvers to override just the bits that they
   support. See
   https://www.minizinc.org/doc-2.3.0/en/fzn-spec.html#solver-specific-libraries
   for more details.
-  There is experimental support for solvers that can read AMPL NL
   files. See
   https://www.minizinc.org/doc-2.3.0/en/solvers.html#non-linear-solvers
   for details.

.. _minor-changes-1:

Minor changes:
^^^^^^^^^^^^^^

-  The JSON input and output has been improved, with full support for
   enums and optional types.
-  A new compiler option -g has been added, which turns variable domain
   changes into constraints (useful for debugging models).
-  The SCIP interface has been updated, with support for indicator
   constraints, bounds disjunctions and a native cumulative constraint.
-  Error reporting has been improved, with location information
   available for errors in par float expressions as well as include
   items.
-  The timeout command line parameter now also applies to compilation
   itself (:bugref:`281`).
-  Operations on par float values are now checked for overflows.
-  The arg_min/arg_max constraints have been improved, with new special
   versions for Boolean variables, and a better standard decomposition.
-  if-then-else-endif expressions with variable conditions are now
   compiled to a predicate call (rather than handled by the compiler),
   which enables solver libraries to implement these as native
   constraints or special decompositions.
-  Dividing a variable by a constant is now translated as a
   multiplication (to keep the constraints linear).
-  A new piecewise_linear predicate has been added to the library to
   make it easier to specify piecewise linear constraints.
-  Print number of solutions as mzn-stat after solving (:bugref:`244`).
-  Make search annotations work for arbitrary array index sets.
-  MiniZinc IDE: The IDE will now check MiniZinc code for syntax and
   type errors, and the editor performs simple code completion for
   MiniZinc keywords
-  MiniZinc IDE: The find/replace dialog is now an inline widget and
   supports incremental search.
-  MiniZinc IDE: Now supports dark mode on macOS.
-  MiniZinc IDE: Add support for extra solver flags (parsed from solver
   configuration).

.. _bug-fixes-8:

Bug fixes:
^^^^^^^^^^

-  Translate let expressions that contain constraints or variables as
   var type-inst. Fixes :bugref:`263`.
-  Fix JSON array parsing by counting elements instead of commas.
-  Fix parsing of the -p flag (:bugref:`271`).
-  Fix type checking for array declarations with single enum type inst
   identifier. E.g. array[$$T] of $U previously matched any
   multi-dimensional array, and now only matches one-dimensional arrays
   with any enum index set.
-  Fix computation of function return type when using type inst
   variables (:bugref:`272`).
-  Evaluate each variable declaration only once in a par let expression.
-  Check domain constraints on variable declarations in par let
   expressions.
-  Try .exe/.bat on windows when using (constructed) absolute paths.
-  Fix array slicing to support empty slices (:bugref:`275`).
-  Fix a bug in the parser that could cause crashes on certain syntax
   errors.
-  Fix the type of bool2int for arrays.
-  Initialise counter for introduced variable ids based on names in
   original model. This avoids reusing variable names if the user model
   contains names such as X_INTRODUCED_0\_.
-  Fix compilation of nested clause/exist constraints, and improve
   handling of negation. Tries to use primitive negation instead of
   creating negated constraints. Should help with half-reification by
   creating more positive contexts.
-  Reorder fields in basic data structures to reduce padding on 64 bit
   platforms (improved memory footprint).
-  Perform type coercion after desugaring array slicing.
-  Translate arguments to bool2int, exists, forall in positive context
   even if those functions are redefined.
-  Don't evaluate par array literals twice (inefficient, and can lead to
   incorrect results when using random number generators).
-  Terminate child processes when minizinc process is terminated by
   signal.
-  Fix function return value array index check for empty arrays (:bugref:`286`).
-  Fix translation of constant false where clause in array
   comprehension.
-  Report error when json multi-dimensional array is not rectangular.
-  Check index sets of function arguments (:bugref:`273`).
-  Ignore partiality variables from CSE table when compiling \_reif and
   \_imp predicates (:bugref:`269`).
-  Flatten comprehensions with variable generators or where conditions
   before evaluating any par functions on them (:bugref:`259`).
-  Add missing redefinitions of basic operators and search annotations
   for optional integers.
-  Resolve filenames given on the command line relative to working
   directory, and warn if file in working directory has same name as
   included file from the library. Fixes :bugref:`276`.
-  Update nosets library with a valid redefinition of set_less over
   booleans.
-  Fix translation of showJSON (:bugref:`294`).
-  Only apply set2array coercion for supported types, otherwise report
   error (:bugref:`295`).
-  Improve special case reasoning for abs on strictly negative
   variables.
-  Add bounds for floating point min/max result in the standard library.
-  MiniZinc IDE: Ensure cursor is visible (editor scrolls to cursor
   position) when pressing tab or enter. Fixes :idebugref:`71` :idebugref:`71`.
-  MiniZinc IDE: Re-dock configuration editor when closing un-docked
   window.
-  MiniZinc IDE: Handle quotes when parsing additional solver command
   line arguments. Fixes :idebugref:`77`.
-  MiniZinc IDE: Add workaround for the missing libnss requirements.
   Fixes :idebugref:`79`.
-  MiniZinc IDE: Allow spaces in $DIR in MiniZincIDE.sh Fixes :idebugref:`81`.

.. _v2.2.3:

`Version 2.2.3 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.2.3>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 31 October 2018)

.. _bug-fixes-9:

Bug fixes:
^^^^^^^^^^

-  Fix some typos in the library documentation.
-  Fix solution checking.
-  Fix line numbers in parsed locations on 64 bit platforms.
-  Fix bounds computation for calls.
-  Fix translation of var where clauses with more than 3 par components.
-  IDE: Only run solution checker if it is enabled in the solver
   configuration dialog.

.. _v2.2.2:

`Version 2.2.2 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.2.2>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 26 October 2018)

.. _changes-6:

Changes:
^^^^^^^^

-  Some changes to the optimisation phase of the compiler, to take into
   account more variables and constraints.
-  Preliminary support for MIP cuts based on graph algorithms (only
   available when compiled with boost C++ libraries; not part of the
   binary distribution).
-  Set Release as default build type when nothing is specified (for
   CMake platforms that do not support multiple build types, like
   Makefiles).
-  Add builtins outputJSON() and outputJSONParameters() for creating an
   array of strings that capture the output and parameters of the model
   as JSON.
-  On Linux and macOS, add /usr/share/minizinc/solvers and
   /usr/local/share/minizinc/solvers to list of paths where solver
   configuration files can be placed.
-  Add OSICBC_INCLUDEDIR and OSICBC_LIBDIR cmake flags.
-  Output search paths for solver configurations using --solvers command
   line option.
-  Add support for Gurobi 8.1
-  Support parsing from stdin and files at the same time.
-  IDE: Add line/column display in status bar.
-  IDE: Optional parameters don't have to be defined in input dialog.
-  IDE: Provide mzn-json-init / mzn-json-init-end handlers to initialise
   HTML window before first solution is produced.
-  IDE: Add version information and minimum system version into
   Info.plist on macOS.
-  IDE: Manage multiple open visualisation windows, and implement
   re-solve function that can be initiated from a visualisation.
-  Binary bundle: Gecode updated to version 6.1.0, Chuffed updated to
   version 0.10.3

.. _bug-fixes-10:

Bug fixes:
^^^^^^^^^^

-  Fix crash when flattening top-level array comprehensions with var
   where clauses.
-  Support input files with more than 1M lines.
-  Special case handling for array literals in top-level foralls:
   flatten in root context.
-  Fix translation of if-then-else for branches with undefined right
   hand sides.
-  Only propagate defines_var annotation to the variable that's actually
   being defined (not others that arise from the same decomposition).
-  Don't flatten arguments of predicates like
   symmetry_breaking_constraint.
-  Remove output_var and output_array annotations from user models
   (these could cause crashes).
-  Fix precedences for weak operators (~+, ~-, ~=, ~*).
-  Fix min and max for opt var arrays to work when the bounds of the
   arrays are unknown.
-  Fix a bug in bounds computations for function calls.
-  Add missing superset FlatZinc builtin.
-  Fix includes in file values.hh for some platforms.
-  Fix a garbage collection issue when printing solutions.
-  Deal with the case that a variable that's required for output is
   assigned to a par variable.
-  Throw type error when an array has only absent values.
-  Flatten all arrays in FlatZinc, also those coming from functional
   definitions.
-  Use list of strings as mzn_solver_path entry in the preferences json
   file.
-  Fix crash when output variable is defined using recursive function
-  IDE: Fix race condition in constructor of HTMLWindow.

.. _v2.2.1:

`Version 2.2.1 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.2.1>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 6 September 2018)

.. _changes-7:

Changes:
^^^^^^^^

-  all_different, all_equal, {int,set,float,bool}_search now accept
   multi-dimensional arrays.
-  Add exponentiation operator (^).
-  Improve layout of generated library documentation for some
   constraints.
-  Relax typechecking to allow assignment of empty array ([]) to
   multi-dimensional array variables. This is required to make empty
   arrays work in JSON data files.
-  Enumerated types can now be initialised using lists of strings. This
   enables enumerated type support in JSON.

.. _bug-fixes-11:

Bug fixes:
^^^^^^^^^^

-  Cumulative constraint for linear solvers now accepts empty arrays.
-  show2d/show3d functions now do not add quotes around array elements
   and work for empty arrays.
-  Add support for slicing of arrays with enumerated types.
-  Fix slicing of 1d arrays.
-  Fix bounds computation for float variable declarations.
-  When FlatZinc solver is terminated due to a timeout, do not report
   this as an error.
-  Fix pretty-printing of multi-dimensional arrays where dimensions
   other than the first one are empty.
-  Add support for where clauses on generator assignment expressions.
-  MiniZinc IDE: Improve dark mode by changing line numbers to dark
   background.
-  MiniZinc IDE: Make parameter input dialog scrollable.
-  MiniZinc IDE: Fix solution compression limit, and output one solution
   per block of compressed solutions.

.. _v2.2.0:

`Version 2.2.0 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.2.0>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 24 August 2018)

This is a major release of MiniZinc, introducing many new features and
improvements.

Major new features:
^^^^^^^^^^^^^^^^^^^

-  **New minizinc command line tool**
   Previous releases contained a ``minizinc`` command line tool that was
   not much more than a simple script that could execute the compiler,
   solver and output processor. The ``minizinc`` executable in version
   2.2.0 is now the main frontend to compilation and solving and
   integrates all of the functionality. It has access to all installed
   MiniZinc solvers (both internal solvers and those interfaced through
   FlatZinc files), and can automatically select the required options
   (e.g., to include the solver-specific MiniZinc globals library).
   You can get a list of available solvers using the ``--solvers``
   command line option, and select a solver using ``--solver``. The
   ``minizinc`` executable can now also be used as a replacement for
   ``mzn2fzn`` (using ``-c``) and ``solns2out`` (using ``--ozn-file``).
-  **Multi-pass compilation**
   The compiler can now perform multiple passes in order to improve the
   target FlatZinc code. This can be controlled using the ``-O`` command
   line flags (``-O0`` to ``-O4``). Multi-pass compilation is
   particularly useful when the target solver requires sophisticated
   decomposition of global constraints (such as for MIP solvers).
-  **Solution checking**
   You can now supply an additional model that will be used to check
   each solution produced by your main model. This can be useful for
   teaching MiniZinc (to give students automatic feedback) and if your
   main model is very complex but checking that a solution is correct is
   easy.
-  **MIP solvers:** support for FICO Xpress, and loading IBM ILOG CPLEX
   as a plugin
   We have added support for FICO Xpress (this requires compiling
   MiniZinc from sources). CPLEX can now be loaded as a plugin, which
   means that the binary distribution of MiniZinc has built-in CPLEX
   support (just bring your own CPLEX dll).
-  **Language extensions**
   The MiniZinc language has been extended with two new features.

   -  Array slicing introduces syntax to conveniently select rows,
      columns or entire slices of arrays. For example, ``x[3,..]``
      selects the third row of array ``x``, while ``x[..,4]`` selects
      the fourth column, and ``x[3..5,2..7]`` selects a slice of rows 3
      to 5 and columns 2 to 7.
   -  Generator expressions can now contain multiple where clauses, e.g.
      ``forall (i in S where foo(i), j in T where i < j) (bar(i,j))``
      This enables more efficient compilation compared to evaluating all
      where clauses in the inner-most generator. In addition to
      iteration (``i in S``), generators can now contain assignment
      expressions (``j=foo(i)``). This enables intermediate definitions
      that can then be used in further generators.

Changes and minor features:
^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  The value of the objective can now be added automatically to the
   output using the ``--output-objective`` command line option. Using
   ``--output-mode dzn``, this allows automatic output of all the free
   variables of the model.
-  Models that do not contain a solve item are now accepted and treated
   as ``solve satisfy``
-  Support for naming constraints and expressions (using ``::"name"``
   syntax)
-  Error messages have been improved, they now contain more accurate
   location information.
-  The compiler can be instructed to accept multiple assignments to the
   same parameter (as long as they are all identical), using the
   ``--allow-multiple-assignments`` command line option.
-  Annotations for supplying warm start values have been added to the
   standard library (currently supported by the MIP solvers Gurobi and
   IBM ILOG CPLEX).
-  The compiler now accepts multiple .mzn files as input.
-  Memory consumption and garbage collection performance has been
   improved.
-  The conditional expression has been extended to support
   ``if <cond> then <exp> endif`` (where ``<exp>`` is bool)
-  Decomposition of one variable type to another (e.g. set into array of
   bool) has been improved.
-  MIP solvers Gurobi and IBM ILOG CPLEX use node files when over 3GB
   working memory
-  Gurobi and CPLEX support the MIPfocus parameter
-  Gurobi supports MiniZinc search annotations by setting fixed
   branching priorities

.. _bug-fixes-12:

Bug fixes:
^^^^^^^^^^

Consult the bug tracker at
https://github.com/MiniZinc/libminizinc/issues

.. _v2.1.7:

`Version 2.1.7 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.1.7>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 10 January 2018)

.. _changes-8:

Changes:
^^^^^^^^

-  Improved linearisation for some element constraints.
-  Improve performance of optimisation phase by using a queue instead of
   a stack.
-  Add --dll option for Gurobi backend to specify the Gurobi DLL to
   load.
-  Add more defines_var annotations.

.. _bug-fixes-13:

Bug fixes:
^^^^^^^^^^

-  Fix generation of variable names in output model (sometimes could
   contain duplicates).
-  Fix enum type inference for array literals with empty sets as their
   first arguments. Fixes :bugref:`180`.
-  Fix incorrect simplification of float domain constraints. Fixes :bugref:`159`.
-  Fix ceil builtin for float values.
-  Add superset decomposition for solvers that do not support set
   variables.
-  Fix three bugs in the garbage collector.
-  Fix a bug in flattening that would create duplicate variables when a
   variable declaration referred to another one in its type-inst.
-  Fix a crash in flattening of partial functions. Fixes :bugref:`187`.
-  Add missing deopt builtins for all par types.
-  Fix output for arrays of sets of enums.
-  Define more functions on par opt types. Fixes :bugref:`188`.
-  Fix type checker to accept arrays of opt set values.
-  Support printing of opt enum types. Fixes :bugref:`189`.
-  Fix evaluation of comprehensions in recursive functions.
-  Fix output of Gurobi backend when used in conjunction with solns2out.
-  Fix pthread linking for mzn-cbc.
-  Catch type error when set literal is declared that contains another
   set.

IDE changes and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^^^^^

-  Fix problem where files with a . in the filename could not be run.
-  Fix font settings (were not saved reliably on some platforms).
-  Enable generic interface for submitting assignments (not just to
   Coursera).
-  Fix output handling for solvers that do not run mzn2fzn.
-  Fix hidden solution display when there are exactly as many solutions
   as the configured threshold for hiding solutions.
-  Add configuration option to print timing information for each
   solution.

.. _v2.1.6:

`Version 2.1.6 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.1.6>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 22 September 2017)

.. _bug-fixes-14:

Bug fixes:
^^^^^^^^^^

-  Fully evaluate parameters before binding formal arguments when
   evaluating call expressions. Fixes :bugref:`177`.
-  Fix incorrect simplification of Boolean constraints assigned to
   variables that are assigned to false.
-  Fix bug in flattening of linear equations that contain the same
   variable on both sides.
-  Fix un-trailing for let expressions, which could sometimes cause
   incorrect code to be emitted when lets are evaluated in nested loops.
   Fixes :bugref:`166`.
-  Fix bug in JSON output of one-dimensional array literals.
-  Fix unification of enum type-inst variables.

.. _v2.1.5:

`Version 2.1.5 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.1.5>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 17 May 2017)

.. _changes-9:

Changes:
^^^^^^^^

-  Some improvements to the linearisation library.
-  Make parser read multiple .mzn files correctly.
-  Enable better bounds computation for array access expressions on
   fixed arrays.
-  Perform better constant folding during optimisation phase. Fixes :bugref:`155`.
-  Don't rewrite pow function into multiplication in the case of power
   of 2.
-  Save some memory by making certain internal data structures more
   compact.
-  Improve source code location of identifiers in generator calls
   (should give more precise error messages).
-  Produce an error message when a comprehension attempts to iterate
   over an infinite set.
-  Produce better error messages for operations on infinite values
   (previously some errors did not contain a source code location).
-  Speed up garbage collection by pre-allocating some memory.

.. _bug-fixes-15:

Bug fixes:
^^^^^^^^^^

-  Fix range check for float literals in arrays.
-  Fix a bug where a constraint could be removed incorrectly. Fixes :bugref:`150`.
-  Include variables for dzn and json output from all included models,
   not just the main model. Fixes :bugref:`153`.
-  Produce multi-dimensional arrays in json output. Fixes :bugref:`156` and :bugref:`157`.
-  Remove incorrect closing bracket from json output. Fixes :bugref:`154`.
-  Fix bounds computation of par int and float arrays.
-  Don't allow var access to arrays of strings or annotations (since
   that would require an element constraint and var string / var ann
   types).
-  Introduce int2float constraints where necessary for some
   linearisations.

.. _v2.1.4:

`Version 2.1.4 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.1.4>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 13 March 2017)

.. _changes-10:

Changes:
^^^^^^^^

-  Add warning for MIP solvers that do not support -a option for
   satisfaction problems.
-  Print introduced variable names with additional underscore to make
   debugging FlatZinc easier. Fixes :bugref:`147`.
-  Add support for pow function in linearisation library.
-  Add support for parallel solving with CBC.
-  Flatten top-level conjunctions in the order defined in the model.

.. _bug-fixes-16:

Bug fixes:
^^^^^^^^^^

-  Fix major race condition that would crash the IDE when it didn't
   detect that a solver process had finished.
-  Improve HTML output in the IDE by making sure every line is
   terminated by a newline.
-  Fix a garbage collection bug that could cause dangling pointers when
   expressions were copied.
-  Fix type checker to allow empty arrays to be assigned to variables
   declared as arrays of enums.
-  Fix infeasibility check in MIP translation for some inequality
   constraints.
-  Improved defines_var annotations for reified xor constraints. Fixes
   :bugref:`146`.
-  Fix output of empty integer sets and deal with empty arrays in output
   models.
-  Fix MIP translation when boolean variables were removed due to
   aliasing.
-  Improve corner cases for linearisation of cumulative constraint.
-  Properly report undefinedness in par bool expressions.
-  Enable some additional constant folding during flattening. Fixes :bugref:`149`.

.. _v2.1.3:

`Version 2.1.3 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.1.3>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 6 February 2017)

.. _changes-11:

Changes:
^^^^^^^^

-  Remove more internal annotations from the generated FlatZinc.
-  Detect failure earlier if optimisation pass leads to fixing of
   variables outside their domains.

.. _bug-fixes-17:

Bug fixes:
^^^^^^^^^^

-  Fix CBC backend to correctly print UNSAT message for models where the
   compiler already detected unsatisfiability, and print solution
   separators even where there is no other output.
-  Add missing var_dom function for arrays of optional integer
   variables. Fixes :bugref:`133`.
-  Fix aliasing for optional integer variables. Fixes :bugref:`132`.
-  Remove all annotations from output model.
-  Fix computation of return type for functions that return arrays of
   enums.
-  Don't output newline if user-defined solution separator or status
   message is empty
-  Fix return type computation for functions where return type contains
   enums.
-  Check finiteness of float literals and bounds. Fixes :bugref:`138`.
-  More checks for function return values. Fixes :bugref:`136`.
-  Fix var int comprehensions (now triggers error message instead of
   crash for var set of int comprehensions). Fixes :bugref:`135`.
-  Fix output of variables with quoted identifiers.
-  Fix flattening of let expressions that contain variables with
   undefined (i.e., partial) right hand side.
-  Make printing of error messages to stdout or stderr more consistent
   across executables.
-  Fix type checking of initialisation of enum types.
-  Improve error messages for array access and index set errors. Fixes
   :bugref:`131`.
-  Fix definition of multi-dimensional element constraints to impose
   correct bounds on index variables.
-  Fix binding analysis during type checking, which did not handle the
   shadowing of top-level declarations by comprehension generators
   correctly. Fixes :bugref:`129`.

.. _v2.1.2:

`Version 2.1.2 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.1.2>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 20 December 2016)

.. _bug-fixes-18:

Bug fixes:
^^^^^^^^^^

-  Fix a bug in the type checking for generators that iterate over
   arrays of enums.
-  Fix a bug in the output handling of arrays of enums.
-  Fix handling of multiple output items (only the last item was
   compiled, now the concatenation is used for output as defined in the
   specification).

.. _v2.1.1:

`Version 2.1.1 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.1.1>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 14 December 2016)

.. _changes-12:

Changes:
^^^^^^^^

-  Add missing min/max functions for set variables. Can be redefined to
   solver builtins using the new redefinitions-2.1.1.mzn library file.
-  Add support for option type expressions as objective functions.
-  Automatically coerce arrays constructed using ++ to any enum index
   set (in addition to array literals and comprehensions).

.. _bug-fixes-19:

Bug fixes:
^^^^^^^^^^

-  Include cmath header to fix compilation issues with some compilers.
   Fixes :bugref:`125`.
-  Fix a garbage collection bug in the type checking for enumerated
   types that would sometimes lead to crashes or incorrect error
   messages.
-  Fix type checking of comprehensions that involve enumerated types.
-  Fix bounds computation for var sets of enumerated types.
-  Support anon_enum function as documented.

.. _v2.1.0:

`Version 2.1.0 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.1.0>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 17 November 2016)

.. _changes-13:

Changes:
^^^^^^^^

-  MiniZinc now supports enumerated types.
-  Solvers can be interfaced directly to the MiniZinc library, and
   MiniZinc comes with direct support for the CBC, Gurobi and CPLEX MIP
   solvers.
-  The linearisation library has been updated, resulting in much better
   FlatZinc being generated for MIP solvers.
-  Data files can be in JSON format, and MiniZinc can produce JSON
   output (using the --output-mode command line option).
-  Variables can be annotated as ::add_to_output instead of writing an
   output item.
-  The compiler can output information about the parameters and output
   variables of a model (using the --model-interface-only option).
-  Floats are handled better (detecting infinities and handling sets of
   floats).
-  Bounds can be computed for more expressions (instead of failing with
   an error message).

.. _bug-fixes-20:

Bug fixes:
^^^^^^^^^^

-  Fix a bug in optimization that could remove variables even if they
   are used. Fixes :bugref:`123`.
-  Fix float variable declarations with sets of floats as domains. Fixes
   :bugref:`117` and :bugref:`98`.
-  Fix type checking and evaluation of asserts with array arguments.
   Fixes :bugref:`109`.
-  Fix abs(var float) declaration to work on floats without declared
   bounds. Fixes :bugref:`106`.
-  Fix a bug in the computation of int and float bounds that could
   result in incorrect bounds in some cases. Fixes :bugref:`94`.
-  Fix garbage collection when creating output models. Fixes :bugref:`77`.
-  Fix binary operators on optional variables (in some cases comparison
   operators were reversed).
-  Fix optimization of unconstrained variables (could sometimes lead to
   constraints being removed although they were not subsumed).

.. _v2.0.14:

`Version 2.0.14 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.0.14>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 31 July 2016)

.. _changes-14:

Changes:
^^^^^^^^

-  Less aggressive aggregation of linear expressions in cases where it
   leads to much less efficient FlatZinc.
-  Don't create temporary variable for an array literal if it is
   discarded immediately anyway.
-  Only create new partiality variable for if-then-else expression if
   there's at least one var condition.
-  Replace recursive definitions of array_intersect and array_union with
   iterative ones.

.. _bug-fixes-21:

Bug fixes:
^^^^^^^^^^

-  Don't report warnings about partiality when using extended generator
   expressions.
-  Include cmath to enable building with some versions of gcc.
-  Constrain result of function call based on function return type if
   necessary.
-  Make sure linear expressions generated during binding of variables
   are properly flattened (including simplification of the linear
   expression)

.. _v2.0.13:

`Version 2.0.13 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.0.13>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 26 March 2016)

.. _bug-fixes-22:

Bug fixes:
^^^^^^^^^^

-  Fix a bug in the Common Subexpression Elimination table of the
   compiler, which could lead to some constraints being dropped
   (especially when using linear redefinitions).
-  The output model sometimes did not include all required definitions,
   in particular when array declarations used identifiers to specify the
   dimensions.
-  The generated FlatZinc sometimes still contained bool variables that
   were not connected to the rest of the model, which could produce
   incorrect solutions being printed.
-  Fix a bug where warnings (e.g. about partial functions) could lead to
   crashes.
-  Fix the bounds computation for integer and float variables, which
   could produce incorrect bounds for linear expressions. Fixes :bugref:`94`.
-  Fix a bug in the IDE that caused solver output to be shown
   incompletely in some cases.

.. _v2.0.12:

`Version 2.0.12 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.0.12>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 25 February 2016)

.. _changes-15:

Changes:
^^^^^^^^

-  Partial functions are now always evaluated in their Boolean context,
   independent of whether they are par or var. If the result of a
   partial function is statically known to be undefined (such as
   division by zero or array access out of bounds), and it is used in a
   constraint expression, this now results in a warning instead of an
   error. Warnings can be turned off using the ::maybe_partial
   annotation. Fixes :bugref:`43` and :bugref:`74`.

.. _bug-fixes-23:

Bug fixes:
^^^^^^^^^^

-  Fix a bug in the optimisation phase related to unification of aliased
   variables.
-  Fix short-circuit evaluation of Boolean expressions.
-  Fix a bug in the optimisation phase related to repeated
   simplification of some Boolean expressions.
-  Handle errors in output produced by solver without solns2out
   crashing. Fixes :bugref:`80`.
-  Fix a bug in the integer bounds computation that caused bool2int with
   an embedded conditional to crash.
-  Fix a problem with short-circuit compilation of == expressions when
   one side was a var opt bool.
-  Stop compilation when model is failed. Fixes a bug where mzn2fzn
   would sometimes not clean up the FlatZinc enough for the solver.

.. _v2.0.11:

`Version 2.0.11 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.0.11>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 15 January 2016)

.. _bug-fixes-24:

Bug fixes:
^^^^^^^^^^

-  Fix parsing of hex and octal literals. Fixes :bugref:`71`.
-  Fix compilation of extended comprehensions. Fixes :bugref:`72`.
-  Fix computation of float array access bounds.
-  Fix aggregation of clauses (could sometimes ignore the negative
   literals).

.. _v2.0.10:

`Version 2.0.10 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.0.10>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 9 December 2015)

.. _bug-fixes-25:

Bug fixes:
^^^^^^^^^^

-  Fix a bug in the optimiser that could lead to undefined variables in
   the generated FlatZinc. Fixes :bugref:`70`.

.. _v2.0.9:

`Version 2.0.9 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.0.9>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 6 December 2015)

.. _bug-fixes-26:

Bug fixes:
^^^^^^^^^^

-  Need to take return type into account when copying functions to
   output model. Fixes :bugref:`55`.
-  Evaluate calls that result in arrays using eval_arraylit. Fixes :bugref:`57`.
-  Move inverse function to its own library file, so that it remains
   available when a solver provides an alternative for the inverse
   predicate.
-  Optimisation phase now recursively checks constraints when elements
   in an array become fixed.
-  Fix CMakeLists file to work for paths that contain spaces.
-  Distinguish between infix operators and regular functions in the
   generated html documentation. Fixes :bugref:`61`.
-  Made parser more robust against incorrect code.
-  Fix increment/decrement operators for IntVals and make all operations
   throw correct overflow exceptions.
-  Fix automatic type coercion for variables declared in let
   expressions.
-  Fix a crash when printing some error messages.
-  Fix compute_div_bounds builtin to return correct result for a
   division by zero.
-  Fix optimisation of Boolean constraints to use pointer equality
   instead of structural equality (same expression can occur multiple
   times in the FlatZinc).
-  Only optimise constraints that have not been removed yet.
-  Fix declaration of functional version of bin_packing_load. Fixes :bugref:`64`.
-  Set type of arrays returned from polymorphic functions. Fixes :bugref:`65`.
-  Fix parsing of quoted unary operator calls.
-  Only compute set functions when bounds are valid. Fixes :bugref:`66`.
-  Compute proper bounds for if-then-else expressions.
-  Report error when no reified version of a constraint is available.
   Fixes :bugref:`67`.
-  Fix type checking of annotations on binary operators.
-  Keep annotations when rewriting linear constraints and remove
   is_defined_var annotations from fixed variables. Fixes :bugref:`69`.

.. _changes-16:

Changes:
^^^^^^^^

Integer, Boolean and float literals are now cached to achieve better
memory performance for some models.

Improve performance of parsing integer literals.

Improve handling of clause constraints.

Add source files of MiniZinc specification to the repository.

Limit maximum array size to enable better error messages.

Add implied_constraint predicate as a synonym for redundant_constraint.

.. _v2.0.8:

`Version 2.0.8 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.0.8>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 19 October 2015)

.. _bug-fixes-27:

Bug fixes:
^^^^^^^^^^

-  Fix incorrect negation of some reified comparisons.
-  Make lb/ub functions work in more cases.
-  Fix several bugs in the optimisation phase (could lead to incorrect
   FlatZinc and crashes).
-  Fix a problem with reverse mapper functions when the result of the
   reverse mapper can be fixed to a constant.

.. _v2.0.7:

`Version 2.0.7 <https://github.com/MiniZinc/MiniZincIDE/releases/tag/2.0.7>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(released 5 October 2015)

.. _changes-17:

Changes:
^^^^^^^^

-  Improved propagation of Boolean constants in the optimisation phase.
   This should result in far fewer aliases and improves simplification
   of conjunctions, disjunctions and clauses.
-  Add special case handling for integer division by 1.

.. _bug-fixes-28:

Bug fixes:
^^^^^^^^^^

-  Fix FlatZinc generator phase, need to turn all array literal
   arguments into 1-based single dimensional arrays.
-  Fix compilation of if-then-else expressions with var conditions
   (which didn't implement proper partiality/totality semantics). Fixes
   :bugref:`42`.
-  Provide correct bounds for weak opt var arithmetic. Fixes :bugref:`51`.
-  Need to be able to handle unflattened annotations. Fixes :bugref:`53`.
-  Fix generation of output model (needs to ignore items that have been
   removed previously).
-  Add missing lb(var set of int) builtin. Fixes :bugref:`47`.
-  Check that var set declarations have a finite element type. Fixes :bugref:`46`.
-  Fix translation context for binary operators on arrays.
-  Need to access IntVal::infinity as a function, otherwise depending on
   linker etc it may become 0 in some cases. Fixes :bugref:`40`.
-  Change pretty printer to use one less digit when printing float
   literals. This fixes :bugref:`41` (or at least
   provides a workaround), but some double constants may still be
   rounded incorrectly when pretty printing and reading them back in.
   The real fix will be to output hex float literals (coming soon).
-  Distinguish between generalised comprehensions (iterating over sets)
   and iterating over arrays. Fixes compilation of comprehensions where
   iteration over an array is combined with var where clauses. Fixes :bugref:`45`.
-  Fix bug in creation of output model where sometimes chains of
   variable definitions could lead to crashes.
-  Avoi creating mutually recursive definitions in some corner cases,
   which could cause the compiler to run into infinite loops.
-  Don't copy vardecl items to output model that are already there.
   Fixes :bugref:`44`.
-  Remove domain from array declarations in FlatZinc (avoids problems
   with domains where holes need to be removed and when there are
   infinities in the domains)
-  Fix flattening of equality operator between non-opt and opt vars.
-  Check that model contains a single solve and output item during type
   checking (previously, multiple output items were not detected and
   resulted in incorrect .ozn files).
-  Fix flattening of xor (arguments need to be in mixed context).
-  Use is_fixed in cumulative definition.
-  Fix bug where a par right hand side of a variable mentioned in the
   output would cause a crash.
-  Fix variable dependency tracking during rewriting in the optimisation
   phase. Could previously lead to variables being removed that are
   still required. Fixes :bugref:`54`.

.. _v2.0.6:

Version 2.0.6
~~~~~~~~~~~~~

(released 2 August 2015)

.. _changes-18:

Changes:
^^^^^^^^

-  Add parser support for hexadecimal floating point constants.

.. _bug-fixes-29:

Bug fixes:
^^^^^^^^^^

-  Fix bounds computation for some calls (abs, int_times).
-  Fix flattening of some array declarations (when right hand side is an
   identifier).
-  Add four missing GC locks (could lead to incorrect garbage
   collection).
-  Compact output model only after optimisation phase (could lead to
   incorrect items being removed from output model).

.. _v2.0.5:

Version 2.0.5
~~~~~~~~~~~~~

(released 31 July 2015)

.. _changes-19:

Changes:
^^^^^^^^

-  Improve the standard decomposition for the cumulative constraint.
-  Better handling of binary operators during type checking and
   flattening, can sometimes avoid stack overflows (e.g. for large
   conjunctions).
-  Make ++ operator left associative (avoid stack overflows in the
   parser).
-  Add ::domain annotations to linear constraints generated from
   multi-dimensional element constraints.
-  Greatly improved linearisation library.

.. _bug-fixes-30:

Bug fixes:
^^^^^^^^^^

-  Fix recursive function calls that contain let expressions.
-  Fix compilation of comprehensions inside parametric functions.
-  Fix a memory leak in solns2out.
-  Fix a problem in the evaluation of binary operators.
-  Fix a bug in the flattening of array literals.
-  Fix a bug that would crash the parser on certain syntax errors in let
   expressions.

.. _v2.0.4:

Version 2.0.4
~~~~~~~~~~~~~

(released 1 July 2015)

.. _changes-20:

Changes:
^^^^^^^^

-  Models can now be read from standard input (using the "-" or
   "--input-from-stdin" command line options). Thanks to Sebastian
   Kosch.
-  Improved handling of bool2int during FlatZinc generation.

.. _bug-fixes-31:

Bug fixes:
^^^^^^^^^^

-  Fix unification of aliased variables which could sometimes result in
   variables being removed although had a constraining right hand side.
-  Fix evaluation of set comprehensions.
-  Fix command line flag --no-output-ozn
-  Fix performance problem when evaluating array expressions inside
   lets.
-  Fix flattening of bool_xor redefinitions.
-  Fix partial evaluation of some array access expressions with var
   indexes.
-  Fix definition of geost constraint.
-  User-defined functions are now copied correctly into the output model
   if they are referenced in the output item.
-  Set comprehensions are fully evaluated.

.. _v2.0.3:

Version 2.0.3
~~~~~~~~~~~~~

(Internal release that did not contain some essential fixes)

.. _v2.0.2:

Version 2.0.2
~~~~~~~~~~~~~

(released 26 May 2015)

.. _changes-21:

Changes:
^^^^^^^^

-  The optimiser now removes simple domain constraints from the FlatZinc
-  The compiler now checks for integer overflows in all built-in
   operations
-  Report an error when the FlatZinc or ozn file cannot be opened for
   writing
-  Add support for 3d array literals (e.g. [\| \|1,2|3,4|,|5,6|7,8\| \|]
   )
-  Add show2d and show3d functions for formatting array output
-  Add row/col functions for variable arrays (fixes :bugref:`2`)
-  Introduce builtins for creating random distributions
-  Add reverse library function
-  Postpone flattening of some reified constraints
-  Slightly improved compilation of partial function calls when it can
   be inferred at compile time that their result is undefined
-  Allow functions with empty argument lists to be declared as function
   int: foo(); instead of just function int: foo;
-  Improve error reporting, in particular for errors in comprehensions
-  Enable expressions a..b where a and b are integer variables
-  Add redundant_constraint and symmetry_breaking_constraint builtins,
   these can be rewritten by solver libraries to allow e.g. local search
   solvers to ignore redundant constraints.
-  Improve flattening of predicates that simply return their arguments
   (makes the redundant_constraint and symmetry_breaking_constraint
   predicates work in more situations).
-  Replace command line option --only-range-domains by optional boolean
   value so that solver libraries can set the flag directly in their
   redefinitions file.
-  Stop flattening immediately when a model has been found to contain an
   inconsistency.
-  Improve flattening of array access expressions, in particular for
   nested array accesses that can be combined into a single element
   constraint
-  Add command line option -s or --statistics to print statistics about
   the generated FlatZinc
-  Improve bounds computation for if-then-else expressions
-  Clause arguments are compiled in positive and negative contexts
   instead of mixed. That means that predicates that introduce free
   variables can now be used in the positive part of a clause.

.. _bug-fixes-32:

Bug fixes:
^^^^^^^^^^

-  Fix simplification of linear expressions, negative coefficients could
   sometimes result in incorrect bounds
-  Fix bounds computation for unary minus operator
-  Add missing par set comparison builtins
-  Fix bounds computation for extended comprehension syntax
-  Fix a bug in the garbage collector that could sometimes lead to
   premature deletion of expressions
-  Fix bounds computation for set difference
-  Fix duplication of some arrays in the FlatZinc (fixes :bugref:`3`)
-  Fix bounds inference for variables bound to empty sets (fixes :bugref:`3`)
-  Fix bug in error reporting function, which would sometimes not report
   the entire call stack
-  Fix the generation of fresh variable names for generator expressions
-  Fix subtype check to allow empty arrays as subtype of arrays of sets
-  Fix crash when using assert/2
-  Fix bug when function used in output referred to par variable
-  Fix bug in type checker, the detection of cyclic definitions was not
   correct and could lead to stack overflows
-  Fix parser to accept expressions with two consecutive array accesses
   (like x[3][4], which are valid MiniZinc if x is an array of sets)
-  Fix error reporting when an evaluation error occurs within a
   comprehension generator
-  Report type error on some ambiguous function calls
-  Report type error on var sets with element type other than int
-  Report type error when trying to coerce a var set into an array
-  Report error when calling function with a value that is outside the
   declared parameter bounds
-  Fix arg_sort builtin to implement the correct semantics
-  Fix sort_by builtin to sort in non-decreasing order, and work with
   floats
-  Fix bug in type checker, now automatic coercions in functions defined
   with type variables (like the comparison operators) work correctly
-  Check that index sets match for arrays assigned in let expressions
-  Fix bug in bounds inference for integer expressions with annotations
-  Fix propagation of defines_var annotation to be pushed through calls
-  Fix parser to accept empty 2d and 3d array literals
-  Fix flattening to remove defines_var annotations with par argument,
   e.g. defines_var(2), which could be introduced by the optimisation
   pass
-  Fix output model creation for variables that have been redefined, and
   remove more unused variables from the FlatZinc.
-  Fix bug in the garbage collector that could result in function items
   not being kept alive in rare cases.

.. _v2.0.1:

Version 2.0.1
~~~~~~~~~~~~~

(released 15 December 2014)

Major bugs and changes:
^^^^^^^^^^^^^^^^^^^^^^^

-  Fix optimisation phase, which was previously incorrectly removing
   variables
-  Add support for trigonometric functions (built-ins were missing in
   2.0.0) and pow (var versions were missing)
-  Fix equality operator on par arrays
-  All expressions in output model are now made par
-  Improve bounds computation for float variables
-  Fix translation of functions that need automatic coercion of their
   return value
-  Fix the array_lb and array_ub builtins, which would return incorrect
   bounds in some cases

Minor bugs and changes:
^^^^^^^^^^^^^^^^^^^^^^^

-  Add space between "array" and "[" in the pretty printer, to be
   compatible with 1.6 output
-  Output all par declarations before the var declarations in FlatZinc
-  Fix parser, which could sometimes crash on invalid input
-  Improve efficiency of bounds computation on some float expressions
-  Add special case handling for division by 1
-  Add missing float_times definition to the flatzinc builtins
-  Use correct version of var_dom for float variables
-  Output information about which files are included in verbose mode
-  Only compute bounds for "then" expressions if the "if" is not fixed
   to false

.. _v2.0.0:

Version 2.0.0
~~~~~~~~~~~~~

(released 9 December 2014)

MiniZinc 2.0 contains many new features and is based on a complete
rewrite of the MiniZinc-to-FlatZinc compiler. If you are currently using
the previous version 1.6, the new tools can be used as drop-in
replacements. The generated FlatZinc is compatible with version 1.6, so
all FlatZinc solvers should work without changes.

MiniZinc language changes
^^^^^^^^^^^^^^^^^^^^^^^^^

-  MiniZinc now supports user-defined functions. Details have been
   published in the paper "MiniZinc with Functions". Both functions and
   predicates can be recursive.
-  MiniZinc now supports option types. Details have been published in
   the paper "Modelling with Option Types in MiniZinc".
-  Let expressions have been generalised. They can now contain
   constraint items in addition to variable declarations.
-  Array index sets can be declared using arbitrary set expressions as
   long as they evaluate to contiguous ranges.
-  The if-then-else expression has been generalised to allow the
   condition to be a var bool expression (instead of only par bool).
-  Array and set comprehensions as well as generator calls can now
   iterate over variables and use var bool where conditions.
-  Any bool expression can now automatically coerce to an int
   expression, likewise for int and float. This means that you don't
   have to write bool2int or int2float in you models any more.
-  Equality constraints can now be posted between array expressions.
-  Arbitrary expressions can now be included ("interpolated") into
   strings, using the syntax "some text \\(e) some more text", where e
   is any expression. It is the same as writing "some text "++show(e)++"
   some more text".

New built-in functions
^^^^^^^^^^^^^^^^^^^^^^

-  Array functions: array1d, arrayXd, row, col, has_index, has_element,
   sort_by, sort, arg_sort, arg_min, arg_max

New global constraints
^^^^^^^^^^^^^^^^^^^^^^

-  arg_max, arg_min
-  arg_sort
-  k-dimensional diffn
-  disjunctive
-  geost
-  knapsack
-  network_flow
-  regular with NFAs
-  symmetric all different
-  optional scheduling constraints: alternative, span, disjunctive,
   cumulative
-  functional versions of many global constraints

New tool chain
^^^^^^^^^^^^^^

-  There are a few new builtins that solvers can reimplement, these are
   listed in the redefinitions-2.0 file.
-  Include items use a different method for finding included files.
   Paths are now interpreted as relative to the file that has the
   include item. That way, the mzn2fzn compiler can be called from a
   different working directory.
-  A new tool, mzn2doc, can produce html output from the documentation
   comments. The MiniZinc distribution contains the documentation for
   global constraints and builtins generated directly from the library
   source code.

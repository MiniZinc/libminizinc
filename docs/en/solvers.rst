.. _ch-solvers:

Solving Technologies and Solver Backends
========================================

The ``minizinc`` tool can use various solver backends for a given model.
Some solvers are separate executables that are called by ``minizinc``;
other solvers are part of the ``minizinc`` binary (either hard-coded or loaded as a dynamic library or "plugin").
In the former case, a temporary FlatZinc file is created and passed to the solver binary;
in the latter one, the flattened model is passed to the backend directly in memory.
Some solvers are part of the binary MiniZinc distribution, others have to be installed separately.
You can find instructions for installing these solvers from source code and integrating into the ``minizinc`` tool
in :ref:`ch-installation_detailed`.
This chapter summarises usage options available for various target solvers.

The help text of ``minizinc`` shows a list of configured solver backends and their tags. You can see solver-specific command-line options by running

.. code-block:: bash

  $ minizinc --help <solver-id or tag>

Constraint Programming Solvers
------------------------------

Constraint Programming is the 'native' paradigm of MiniZinc. Below we discuss most common CP solvers.
For their performance, consult MiniZinc Challenges (https://www.minizinc.org/challenge/).

Gecode
~~~~~~

Gecode is an open-source constraint programming system (see https://www.gecode.org).
It supports many of MiniZinc's global constraints natively, and has support for integer, set and float variables.

Gecode supports a number of constraints and search annotations that are not part of the MiniZinc standard library.
You can get access to these by adding :mzn:`include "gecode.mzn";` to your model. The additional declarations are documented in :numref:`ch-lib-gecode`.


Chuffed
~~~~~~~

Chuffed is a constraint solver based on *lazy clause generation* (see https://github.com/chuffed/chuffed).
This type of solver adapts techniques from SAT solving, such as conflict clause learning,
watched literal propagation and activity-based search heuristics, and can often be much faster than traditional CP solvers.

In order to take full advantage of Chuffed's performance,
it is often useful to add a search annotation to the model (see :ref:`sec-search`),
but allow Chuffed to switch between this defined search and its activity-based search.
In order to enable this behaviour, use the ``-f`` (free search) command line option or
select *Free search* in the solver configuration pane of the MiniZinc IDE.

Chuffed supports a number of additional search annotations that are not part of the MiniZinc standard library.
The additional declarations are documented in :numref:`ch-lib-chuffed`.

OR-Tools
~~~~~~~~

OR-Tools is a powerful open-source CP/SAT/LP solver (see https://developers.google.com/optimization/).
It supports many of MiniZinc's global constraints natively. It often performs better multi-threaded (option ``-p``)
so it can employ various solving technologies. A search annotation (see :ref:`sec-search`) can be useful,
however allowing OR-Tools to mix the prescribed strategy with its own (option ``-f``) usually is best,
analogously to Chuffed.


Mixed-Integer Programming Solvers
---------------------------------

MiniZinc has built-in support for Mixed Integer Programming solvers.
If you have any MIP solver installed (and MiniZinc was compiled with its support),
you can run a model using MIP like this on the command line:

.. code-block:: bash

  minizinc --solver mip -v -s -a model.mzn data.dzn

Of course, you can also select a particular solver, e.g. Gurobi (in case it is available):

.. code-block:: bash

  minizinc --solver gurobi -v -s -a model.mzn data.dzn

MIP-Aware Modeling (But Mostly Useful for All Backends)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Avoid mixing positive and negative coefficients in the objective. Use 'complementing' variables to revert sense.

Avoid nested expressions which are hard to linearize (decompose for MIP). For example, instead of

.. code-block:: minizinc

  constraint forall(s in TASKS)(exists([whentask[s]=0] ++
    [whentask[s]>= start[s]+(t*numslots) /\ whentask[s]<=stop[s]+(t*numslots) | t in 0..nummachines-1]));

prefer the tight domain constraint

.. code-block:: minizinc

  constraint forall(s in TASKS)(whentask[s] in
    {0} union array_union([ start[s]+(t*numslots) .. stop[s]+(t*numslots) | t in 0..nummachines-1]));

To avoid **numerical issues**, make variable domains as tight as possible (compiler can deduce bounds in certain cases but explicit bounding can be stronger).
Try to keep magnitude difference in each constraint below 1e4.
Especially for variables involved in logical constraints, if you cannot reduce the domains to be in +/-1e4,
consider indicator constraints (available for some solvers, see below), or use the following trick:
instead of saying :mzn:`b=1 -> x<=0` where x can become very big, use e.g. :mzn:`b=1 -> 0.001*x<=0.0`.
Especially for integer variables, the domain size of 1e4 should be an upper bound if possible -- what is the value of integrality otherwise?
Avoid large coefficients too, as well as large values in the objective function.
See more on tolerances in the Solver Options section.

Example 1: *basic big-M constraint vs implication*. Instead of :mzn:`<expr> <= 1000000*y` given :mzn:`var 0..1: y`
and where you use the 'big-M' value of 1000000 because you don't know a good upper bound on :mzn:`<expr>`, prefer :mzn:`y=0 -> <expr> <= 0`
so that MiniZinc computes a possibly tighter bound, and consider the above trick: :mzn:`y=0 -> 0.0001*<expr> <= 0.0` to reduce magnitudes.

Example 2: *cost-based choice*. Assume you want the model to make a certain decision, e.g., constructing a road, but then its cost should be minimal among some others, otherwise not considered. This can be modeled as follows:

.. code-block:: minizinc

  var 0..1: c;                                             %% Whether we construct the road
  var int: cost_road = 286*c + 1000000*(1-c);
  var int: cost_final = min( [ cost_road, cost1, cost2 ] );

Note the big coefficient in the definition of :mzn:`cost_road`. It can lead to numerical issues and a wrong answer: when the solver's integrality tolerance is 1e-6, it can assume :mzn:`c=0.999999` as equivalent to :mzn:`c=1` leading to :mzn:`cost_road=287` after rounding.

A better solution, given reasonable bounds on :mzn:`cost1` and :mzn:`cost2`, is to replace the definition as follows:

.. code-block:: minizinc

  int: cost_others_ub = 1+2*ub_array( [cost1, cost2] );    %% Multiply by 2 for a stronger LP relaxation
  var int: cost_road = 286*c + cost_others_ub*(1-c);


Useful Flattening Parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following parameters can be given on the command line or modified in ``share/minizinc/linear/options.mzn``:

::

  -D nSECcuts=0/1/2                            %% Subtour Elimination Constraints, see below
  -D fMIPdomains=true/false                    %% The unified domains feature, see below
  -D float_EPS=1e-6                            %% Epsilon for floats' strict comparison,
                                               %% used e.g. for the following cases:
                                               %% x!=y, x<y, b -> x<y, b <-> x<=y
  -DfIndConstr=true -DfMIPdomains=false        %% Use solver's indicator constraints, see below
  -DMinMaxGeneral=true                         %% Send min/max constraints to the solver (Gurobi only)
  -DQuadrFloat=false -DQuadrInt=false          %% Not forward float/integer multiplications for MIQCP backends, see below
  -DUseCumulative=false                        %% Not forward cumulative with fixed durations/resources (SCIP only)
  -DUseOrbisack=false                          %% Not forward lex_lesseq for binary/bool vectors (SCIP only)
  -DOrbisackAlwaysModelConstraint=true         %% lex_lesseq ignores being in symmetry_breaking_constraint() (SCIP only)
                                               %% Required for SCIP 7.0.2, or use patch: http://listserv.zib.de/pipermail/scip/2021-February/004213.html
  -DUseOrbitope=false                          %% Not forward lex_chain_lesseq for binary/bool matrices (SCIP only)
  --no-half-reifications                       %% Turn off half-reification (full reification was until v2.2.3)

Some Solver Options and Changed Default Values
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following command-line options affect the backend or invoke extra functionality. Note that some of them have default values which may be different from the backend's ones.
For example, tolerances have been tightened to enable more precise solving with integer variables and objective. This slightly deteriorates performance on average, so when your model has moderate constant and bound magnitudes, you may want to pass negative values to use solver's defaults.

::

  -h <solver-tag>    full description of the backend options
  --relGap <n>       relative gap |primal-dual|/<solver-dep> to stop. Default 1e-8, set <0 to use backend's default
  --feasTol <n>      primal feasibility tolerance (Gurobi). Default 1e-8
  --intTol <n>       integrality tolerance for a variable. Default 1e-8
  --solver-time-limit-feas <n>, --solver-tlf <n>
                     stop after <n> milliseconds after the first feasible solution (some backends)
  --writeModel <file>
                     write model to <file> (.lp, .mps, .sav, ...). All solvers support the MPS format
                     which is industry standard. Most support the LP format. Some solvers have own formats,
                     for example, the CIP format of SCIP ("constraint integer programming").
  --readParam <file>
                     read backend-specific parameters from file (some backends)
  --writeParam <file>
                     write backend-specific parameters to file (some backends)
  --readConcurrentParam <file>
                     each of these commands specifies a parameter file of one concurrent solve (Gurobi only)
  --keep-paths       this standard flattening option annotates every item in FlatZinc by its "flattening history".
                     For MIP solvers, it additionally assigns each constraint's name as the first 255 symbols of that.
  --cbcArgs '-guess -cuts off -preprocess off -passc 1'
                     parameters for the COIN-OR CBC backend

All MIP solvers directly support multi-threading (option ``-p``). For COIN-BC to use it, it needs to be
configured with ``--enable-cbc-parallel``.

Subtour Elimination Constraints
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Optionally use the SEC cuts for the circuit global constraint.
Currently only Gurobi, IBM ILOG CPLEX, and COIN-OR CBC (trunk as of Nov 2019).
If compiling from source, this needs boost and cmake flag ``-DCOMPILE_BOOST_MINCUT=ON``
(or ``#define`` it in ``lib/algorithms/min_cut.cpp``).
Compile your model with the flag ``-DnSECcuts=<n>`` with the following possible ``<n>``:
0,1: use MTZ formulation; 1,2: pass on circuit constraints
to the SEC cut generator, so 1 would use both.

Unified Domains (MIPdomains)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The 'MIPdomains' feature of the Flattener aims at reducing the number of binary flags
encoding linearized domain constraints, see the paper
*Belov, Stuckey, Tack, Wallace. Improved Linearization of Constraint Programming Models. CP 2016.*

By default it is on.
To turn it off which might be good for some models, add option ``-D fMIPdomains=false`` during flattening.
Some parameters of the unification are available, run with ``--help``.

Indicator Constraints
~~~~~~~~~~~~~~~~~~~~~

Some solvers (IBM ILOG CPLEX, Gurobi, SCIP) have indicator constrains with greater numerical stability than big-M decomposition.
Moreover, they can be applied to decompose logical constraints on *unbounded variables*. However, for reified comparisons with
reasonable big-M bounds they perform worse because solvers don't include them in the LP relaxation.
Add command-line parameters ``-D fIndConstr=true -D fMIPdomains=false`` when flattening
to use them.

Quadratic Constraints and Objectives (MIQCP)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Gurobi 9.0 and SCIP support MIQCP (invoking non-convex global optimizer because MiniZinc translates multiplication to
equality with an intermediate variable: whenever the model uses an expression x*y it is converted to z with z==x*y which is non-convex).
While this is mostly advantageous for integer multiplication (which is linearly decomposed for other solvers), for float variables
this is the only way to go. To switch off forwarding float/integer multiplications to the backend, run compiler with either or both of
``-DQuadrFloat=false -DQuadrInt=false``.

Pools of User Cuts and Lazy Constraints
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Some constraints in the model can be declared as user and/or lazy cuts and they will be added to the corresponding pools
for the solvers supporting them. For that, apply annotations :mzn:`::MIP_cut` and/or :mzn:`::MIP_lazy` after a constraint.
For Gurobi and IBM ILOG CPLEX, see ``share/minizinc/linear/options.mzn`` for their exact meaning.

Warm Starts
~~~~~~~~~~~

For general information of warm start annotations, see :ref:`sec_warm_starts`.
Warm starts are currently implemented for Gurobi, IBM ILOG CPLEX, Xpress, and COIN-OR CBC.

.. _ch-solvers-nonlinear:

Non-Linear Solvers via NL File Format
-------------------------------------

MiniZinc has experimental support for non-linear solvers that conform to the AMPL NL standard. There are a number of open-source solvers, such as Ipopt, Bonmin and Couenne, that can be interfaced to MiniZinc in this way.

You can download binaries of these solvers from AMPL (https://ampl.com/products/solvers/open-source/). In order to use them with MiniZinc, you need to create a solver configuration file. Future version of MiniZinc will make this easier, but for now you can follow these steps:

1. Download the solver binary. For this example, we assume you chose the Couenne solver, which supports non-linear, non-convex, mixed discrete and continuous problems.
2. Create a solver configuration file called ``couenne.msc`` in the ``share/minizinc/solvers`` directory of your MiniZinc installation, with the following contents:

  .. code-block:: json

    {
      "id" : "org.coin-or.couenne",
      "name" : "Couenne",
      "executable" : "/Users/tack/Downloads/couenne-osx/couenne",
      "version": "0.5.6",
      "supportsFzn":false,
      "supportsNL":true
    }

  You can adapt the ``version`` field if you downloaded a different version (it's only used for displaying).

3. Run ``minizinc --solvers``. The Couenne solver should appear in the list of solvers now.
4. Run ``minizinc --solver couenne model.mzn`` on some MiniZinc model, or use Couenne from the MiniZinc IDE.

The AMPL NL support is currently experimental, and your MiniZinc model is translated to NL without regard
for the capabilities of the target solver. For example, Ipopt only supports continuous variables, so translating
a model with integer variables will result in a solver-level error message. There is currently no support for
translating Boolean variables and constraints into 0/1 integer variables (as required by e.g. Couenne).
You can experiment with the standard linearisation library, using the ``-Glinear [-DQuadrFloat=true -DQuadrInt=true]``
flag. However, this will either
linearise all integer constraints, even the ones that solvers like Couenne may support natively, or use non-convex
representation. We will ship dedicated solver libraries for some NL solvers with future versions of MiniZinc.

.. _ch-solvers-fznso:

Solvers Loaded as a Library (FZnSO)
-----------------------------------

A solver that implements the FZnSO interface is a shared library rather than an
executable. MiniZinc loads it at run time and runs it directly on the flat
model, so nothing is written to a FlatZinc file and no process is started.

Such a solver *declares* what it can do — its options, the constraints it
implements, the types of decision variable it supports and the statistics it
reports — so its configuration file needs to name nothing but the library:

.. code-block:: json

  {
    "id": "org.gecode.gecode-fznso",
    "name": "Gecode (FZnSO)",
    "version": "6.4.0",
    "inputType": "FZNSO",
    "executable": "/path/to/lib/fznso/libgecode.dylib"
  }

The ``executable`` field names the library: either a path, or a bare name such
as ``gecode``, which is looked up on the FZnSO search path
(``$FZNSO_SOLVER_PATH``, then the per-user and system ``fznso`` directories).

What MiniZinc derives from the declaration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Command line flags.** Every option the solver declares becomes a flag. The
options in the FZnSO registry map onto the standard MiniZinc flags —
``all_solutions`` to ``-a``, ``intermediate`` to ``-i``, ``fixed_search`` to
``-f``, ``threads`` to ``-p``, ``random_seed`` to ``-r``, ``time_limit`` to
``-t`` and ``verbose`` to ``-v`` — and every other option becomes
``--<option>``. They are listed by ``minizinc --help <solver id>`` and reported
by ``--solvers-json``, so the IDE offers them too. Nothing needs an
``extraFlags`` entry in the configuration file.

**Globals.** A constraint the solver declares enables the matching
``fzn_<constraint>``: the standard library's decomposition of it is dropped, and
the call is passed to the solver instead. A solver that only wants to say "I
implement ``all_different_int``" therefore needs no MiniZinc library at all. A
solver that *does* ship an ``mznlib`` still wins wherever it provides a
redefinition, because only declarations MiniZinc itself supplies are dropped.

**Constraints only implemented for fixed arguments.** A propagator often exists
only for the fixed case — unary scheduling needs fixed durations, an integer
power needs a fixed exponent. A solver says so by declaring the constraint with
those arguments fixed::

  disjunctive_strict(list of var int, list of int)

The decomposition is then *kept*, and the constraint gains a second declaration
for the fixed case, with MiniZinc generating the dispatch between them::

  predicate fzn_disjunctive_strict(array [int] of var int: s, array [int] of int: d);
  predicate fzn_disjunctive_strict(array [int] of var int: s, array [int] of var int: d) =
    if is_fixed(d) then fzn_disjunctive_strict(s, fix(d)) else <decomposition> endif;

The guard is ``is_fixed``, not overload resolution, so it also catches an
argument that is a variable with a singleton domain.

A decomposition is dropped during type checking, before the compiler
monomorphises. That ordering matters: a parameter-type copy is only made of a
function that still has a body, so a declaration the solver implements never
gains one. Without it a call whose arguments are all fixed would resolve to the
copy — which returns ``par bool`` and so can never become a constraint — rather
than to what the solver implements. ``complete()`` in
``experimental/on_restart`` is the case that shows it: it reaches
``fzn_on_restart_complete`` with a fixed argument, and the standard library's
body for that is an ``abort``.

**Set variables.** A solver that declares no ``var set of int`` gets
``nosets.mzn``, which rewrites set variables into arrays of Booleans. A model
that includes ``nosets.mzn`` itself keeps it: what that file redefines is never
dropped, because a solver saying it accepts ``set_lt`` is not an answer to a
model that has asked for no set variables at all.

**The solver's own constraints.** A solver usually implements more than the
standard library has names for. Those are reached by including a file named
after the solver::

  include "gecode.mzn";
  constraint array_set_element_intersect(sel, ys, z);

If the solver ships an ``mznlib`` containing ``gecode.mzn``, that file is used
and anything it does not declare is added to it. If it ships nothing, the whole
file is synthesised from the declared constraint list. Either way the model has
to ask: without the include the identifier is undefined, so a model that
depends on a particular solver says so.

Only constraints come across this way — annotations and reverse-mapping
functions are not part of what a solver declares, so a solver wanting to offer
those still ships a library for them.

**The overlay library.** ``share/minizinc/fznso`` is searched before the
standard library (and after any configured ``mznlib``). It gives the
declarations a global lowers to shapes a solver can actually match — a flat
tuple array for ``fzn_table_int``, a flat transition matrix for ``fzn_regular``,
``fzn_no_overlap`` in place of ``fzn_diffn``, which is the identifier the FZnSO
constraint specification standardises for it.

Where a constraint reads one array's values as indices into another —
``fzn_inverse``, ``fzn_int_set_channel``, ``fzn_bin_packing_load``,
``fzn_range``, ``fzn_roots``, ``fzn_cumulatives`` and the rest — the index the
array is numbered from travels next to it as a plain ``int``, because the flat
array cannot carry an index set. A solver meets such an offset by padding or
shifting its own array, which it can only do from zero up, so the
``array [int] of`` overload keeps the decomposition for a caller numbering from
below zero.

It deliberately stops short of the FlatZinc builtin layer. The standard library
declares some builtins body-less, and does not mention the half-reified
(``_imp``) forms at all, because a solver is expected to ship a library saying
which of them it supports and decomposing the rest. That is still the way to do
it: a ``redefinitions*.mzn`` in the solver's ``mznlib``. A solver that ships
none gets neither half-reification nor those decompositions.

Every predicate a solver may implement is declared with ``list of`` rather than
``array [int] of``. A FlatZinc array is one-based, and ``list of`` is what says
so: the type-checker holds the caller to an index set starting at ``1``, so the
predicate body and the solver agree on the numbering. The overload the standard
library's globals call keeps their signature and rebases with ``array1d``.

``list of T`` and ``array [int] of T`` are one type to the overload resolver, so
the two forms can only coexist when they differ in arity, in the dimension of
some argument, or in the position and name of one. ``fzn_cumulatives`` therefore
takes its machine offset in the middle and calls its bound array ``capacity``,
and ``fzn_diffn`` is renamed rather than overloaded.

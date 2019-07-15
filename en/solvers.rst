.. _ch-solvers:

Solving Technologies and Solver Backends
========================================

The ``minizinc`` tool can use various solver backends for a given model.
Some solvers are separate executables that are called by ``minizinc`` and passed the name of a FlatZinc file;
other solvers are part of the ``minizinc`` binary (either hard-coded or loaded as a dynamic library or "plugin").
Some solvers are part of the binary MiniZinc distribution, others have to be installed separately.
You can find instructions for installing these solvers from source code and integrating into the ``minizinc`` tool
in :ref:`ch-installation_detailed`.
This chapter summarises usage options available for various target solvers.

The help text of ``minizinc`` shows a list of configured solver backends and their tags. You can see solver-specific command-line options by running

.. code-block:: bash

  $ minizinc --help <solver-id or tag>

Constraint Programming Solvers
------------------------------

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

Chuffed supports a number of additional search annotations that are not part of the MiniZinc standard library. The additional declarations are documented in :numref:`ch-lib-chuffed`.

OR-Tools
~~~~~~~~

OR-Tools is an open-source CP/SAT/LP solver (see https://developers.google.com/optimization/).
It supports many of MiniZinc's global constraints natively. It often performs better multi-threaded (option ``-p``)
so it can employ various solving technologies. A search annotation (see :ref:`sec-search`) can be useful.


Mixed-Integer Programming Solvers
---------------------------------

MiniZinc has built-in support for Mixed Integer Programing solvers. If you have any MIP solver installed (and MiniZinc was compiled with MIP support), you can run a model using MIP like this on the command line:

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
Avoid large coefficients too, as well as large values in the objective function. See more on tolerances in a below section.

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
  -D float_EPS=1e-6                            %% Epsilon for floats' strict comparison
  -DfIndConstr=true -DfMIPdomains=false        %% Use solver's indicator constraints, see below
  --no-half-reifications                       %% Turn off halfreification (full reification was until v2.2.3)

Some Solver Options and Changed Default Values
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following command-line options affect the backend or invoke extra functionality. Note that some of them have default values which may be different from the backend's ones.
For example, tolerances have been tightened to enable more precise solving with integer variables and objective. This slightly deteriorates performance on average, so when your model has moderate constant and bound magnitudes, you may want to pass negative values to use solver's defaults.

::

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
  --cbcArgs '-guess -cuts off -preprocess off -passc 1'
                  parameters for the COIN-OR CBC backend

All MIP solvers directly support multi-threading (option ``-p``). For COIN-BC to use it, it needs to be
configured with ``--enable-cbc-parallel``.
For other command-line options, run ``minizinc -h <solver-id>``.

Subtour Elimination Constraints
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Optionally use the SEC cuts for the circuit global constraint.
Currently only Gurobi and IBM ILOG CPLEX (2019/03).
If compiling from source, this needs boost and cmake flag ``-DCOMPILE_BOOST_MINCUT=ON``
(or ``#define`` it in ``lib/algorithms/min_cut.cpp``).
Values of ``nSECcuts``: 0,1: use MTZ formulation; 1,2: pass on circuit constraints
to the SEC cut generator, so 1 would use both.

Unified Domains (MIPdomains)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The 'MIPdomains' feature of the Flattener aims at reducing the number of binary flags
encoding linearized domain constraints, see the paper
*Belov, Stuckey, Tack, Wallace. Improved Linearization of Constraint Programming Models. CP 2016.*

By default it is off since v2.3.0.
To turn it on, add option ``-D fMIPdomains=true`` during flattening.
Some parameters of the unification are available, run with ``--help``.

Indicator Constraints
~~~~~~~~~~~~~~~~~~~~~

Some solvers (IBM ILOG CPLEX, Gurobi, SCIP) have indicator constrains with greater numerical stability than big-M decomposition.
Moreover, they can be applied to decompose logical constraints on *unbounded variables*.
Add command-line parameters ``-D fIndConstr=true -D fMIPdomains=false`` when flattening
to use them.

Pools of User Cuts and Lazy Constraints
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Some constraints in the model can be declared as user and/or lazy cuts and they will be added to the corresponding pools
for the solvers supporting them. For that, apply annotations :mzn:`::MIP_cut` and/or :mzn:`::MIP_lazy` after a constraint.
For Gurobi and IBM ILOG CPLEX, see ``share/minizinc/linear/options.mzn`` for their exact meaning.

Warm Starts
~~~~~~~~~~~

For general information of warm start annotations, see :ref:`sec_warm_starts`.
Warm starts are currently implemented for Gurobi, IBM ILOG CPLEX, and XPRESS.

.. _ch-solvers-nonlinear:

Non-linear Solvers
------------------

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

The AMPL NL support is currently experimental, and your MiniZinc model is translated to NL without regard for the capabilities of the target solver. For example, Ipopt only supports continuous variables, so translating a model with integer variables will result in a solver-level error message. There is currently no support for translating Boolean variables and constraints into 0/1 integer variables (as required by e.g. Couenne). You can experiment with the standard linearisation library, using the ``-Glinear`` flag. However, this will linearise all integer constraints, even the ones that solvers like Couenne may support natively (it does allow you to use non-linear constraints on float variables, though). We will ship dedicated solver libraries for some NL solvers with future versions of MiniZinc.


















.. _ch-solvers:

Solving Technologies and Solver Backends
========================================

The ``minizinc`` tool can use various solver backends for a given model. The solver interface can use intermediate FlatZinc files or direct solver interfaces. This chapter summarises usage options available for various target solvers.

The help text of ``minizinc`` shows a list of configured solver backends and their tags. You can see solver-specific command-line options by running

.. code-block:: bash

  $ minizinc --help <solver-id or tag>

Constraint Programming Solvers
------------------------------

Mixed-Integer Programming Solvers
---------------------------------

All MIP solvers directly support multi-threading (option ``-p``). For this, COIN-BC needs to be
configured with ``--enable-cbc-parallel``. Use ``svn/git`` to get the latest stable CBC revision,
see ``https://projects.coin-or.org/Cbc``, currently ``https://projects.coin-or.org/svn/Cbc/stable/2.10``.

Calling a solver on a MiniZinc model directly:

.. code-block:: bash
  
  $ minizinc --solver -v -s -a model.mzn data.dzn

or separated flattening+solving - sometimes more stable but slower due to file I/O:

.. code-block:: bash
  
  $ minizinc --solver coin-bc -c model.mzn data.dzn && minizinc --solver cbc -v -s -a model.fzn | minizinc --ozn-file model.ozn

Installation of MIP Backends
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SCIP (as of 6.0.1.0): you can also install into another location as the default ``make install``,
but then use minizinc's ``-DCMAKE_PREFIX_PATH=...`` to let CMake find that location.

.. code-block:: bash
  $ tar xvfz scipoptsuite-6.0.1.tgz
  $ cd scipoptsuite-6.0.1
  $ cd soplex
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make -j5
  $ cd ../scip
  $ mdkir build
  $ cd build
  $ cmake .. -DSOPLEX_DIR=~/Downloads/Software/scipoptsuite-6.0.1/soplex/build
  $ make -j5
  $ sudo make install                    ## Now MZN should find it

COIN-OR CBC (as of 2.10/stable. Prefer stable or even trunk):

.. code-block:: bash
  $ svn checkout https://projects.coin-or.org/svn/Cbc/stable/2.10/ Cbc-stable
  $ cd Cbc-stable
  $ ./configure <--enable-cbc-parallel>
  $ make && make install
  $ export CBC_HOME=$(pwd)               ## put this into .profile with $(pwd) expanded


Useful Flattening Parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following parameters can be given on the command line or modified in ``share/minizinc/linear/options.mzn``:

.. option::  -D nSECcuts=0/1/2                %% Subtour Elimination Constraints, see below
.. option::  -D fMIPdomains=true/false        %% The unified domains feature
.. option::  -D float_EPS=1e-6                %% Epsilon for floats' strict comparison
.. option::  -DfIndConstr=true -DfMIPdomains=false        %% Use solver's indicator constraints, see below

Some Solver Options and Changed Default Values
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following command-line options affect the backend or invoke extra functionality. Note that some of them have default values which may be different from the backend's ones.
For example, tolerances have been tightened to enable more precise solving with integer variables and objective. This deteriorates performance on average, so when your model has moderate constant and bound magnitudes, you may want to pass negative values to use solver's defaults.

.. option::  --relGap <n>

    relative gap |primal-dual|/<solver-dep> to stop. Default 1e-8, set <0 to use backend's default

.. option::    --intTol <n>

    integrality tolerance for a variable. Default 1e-6

.. option::    --solver-time-limit-feas <n>, --solver-tlf <n>

    stop after <n> milliseconds after the first feasible solution (some backends)

.. option::    --writeModel <file>

    write model to <file> (.lp, .mps, .sav, ...)

.. option::  --readParam <file>

    read backend-specific parameters from file

.. option::  --writeParam <file>

    write backend-specific parameters to file

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
encoding linearized domain constraints, see

    Belov, Stuckey, Tack, Wallace. Improved Linearization of Constraint Programming Models. CP 2016.

By default it is on, but for some models such as packing problems, it is better off.
To turn it off, add option ``-D fMIPdomains=false`` during flattening.
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
for the solvers supporting them. For that, apply annotations ``::MIP_cut`` and/or ``::MIP_lazy`` after a constraint.
For Gurobi and IBM ILOG CPLEX, see ``share/minizinc/linear/options.mzn`` for their exact meaning.

Warm Starts
~~~~~~~~~~~

For general information of warm start annotations, see Tutorial.
Warm starts are currently implemented for Gurobi and IBM ILOG CPLEX.


.. _ch-solvers:

Solving Technologies and Solver Backends
========================================

The ``minizinc`` tool can use various solver backends for a given model. The solver interface can use intermediate FlatZinc files or direct solver interfaces. This chapter summarises usage options available for various target solvers.

The help text of ``minizinc`` shows a list of configured solver backends and their tags. You can see solver-specific command-line options by running

.. code-block:: bash

  $ minizinc --help <solver-id or tag>

Constraint Programming Solvers
------------------------------

Gecode
~~~~~~

To compile from source, run ``./configure --disable-qt && make -j8 && sudo make install`` which installs in the standard location. 
For ``minizinc`` to see it, add ``gecode.msc`` in an appropriate location (see Solver Config Files) containing the following:

.. code-block:: json

 {
  "id": "org.gecode.gecode",
  "name": "Gecode",
  "description": "Gecode FlatZinc executable",
  "version": "6.2.0",
  "mznlib": "/usr/local/share/gecode/mznlib",
  "executable": "fzn-gecode",
  "tags": ["cp","int", "float", "set", "restart"],
  "stdFlags": ["-a","-f","-n","-p","-r","-s","-t"],
  "supportsMzn": false,
  "supportsFzn": true,
  "needsSolns2Out": true,
  "needsMznExecutable": false,
  "needsStdlibDir": false,
  "isGUIApplication": false
 }


Chuffed
~~~~~~~

Chuffed's performance is usually much better with option ``-f`` (free search).

You can obtain Chuffed's source from https://github.com/chuffed/chuffed. After compiling and installing it by 

.. code-block:: bash

  $ mkdir build && cd build && cmake .. && cmake --build . -- -j8 && sudo cmake --build . --target install

you can configure it for ``minizinc`` using the file ``chuffed.msc`` with the following content:

.. code-block:: json

 {
  "id": "org.chuffed.chuffed",
  "name": "Chuffed",
  "description": "Chuffed FlatZinc executable",
  "version": "${chuffed_VERSION_MAJOR}.${chuffed_VERSION_MINOR}.${chuffed_VERSION_PATCH}",
  "mznlib": "/usr/local/share/chuffed/mznlib",
  "executable": "fzn-chuffed",
  "tags": ["cp","lcg","int"],
  "stdFlags": ["-a","-f","-n","-r","-s","-t","-v"],
  "supportsMzn": false,
  "supportsFzn": true,
  "needsSolns2Out": true,
  "needsMznExecutable": false,
  "needsStdlibDir": false,
  "isGUIApplication": false
 }


Mixed-Integer Programming Solvers
---------------------------------

Calling a MIP solver on a MiniZinc model directly:

.. code-block:: bash
  
  $ minizinc --solver mip -v -s -a model.mzn data.dzn

or separated flattening+solving - sometimes more stable but slower due to file I/O:

.. code-block:: bash
  
  $ minizinc --solver cbc -c model.mzn data.dzn && minizinc --solver cbc -v -s -a model.fzn | minizinc --ozn-file model.ozn

MIP-Aware Modeling (But Mostly Useful for All Backends)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Avoid mixing positive and negative coefficients in the objective. Use 'complementing' variables to revert sense.

To avoid numerical issues, make variable domains as tight as possible (compiler can deduce bounds in certain cases but explicit bounding can be stronger).
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

Installation of MIP Backends: *SCIP*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For *SCIP (as of 6.0.1.0)*, if you download the Optimization Suite, the installation commands should be as follows.

1. Download the SCIP Optimization Suite 6.0.1 (or higher) source code: https://scip.zib.de/download.php?fname=scipoptsuite-6.0.1.tgz

2. Untar it and change directories into scipoptsuite-6.0.1

3. create a build directory and change directories there

4. Execute 

.. code-block:: bash

    cmake .. -DCMAKE_BUILD_TYPE=Release [-DCMAKE_INSTALL_PREFIX=/home/user/local/scip/installation]

The first flag is necessary, the second one is optional in order to install SCIP and SoPlex non-systemwide.

5. Compile and install SoPlex, SCIP, and its optional components:

.. code-block:: bash

    make && make install

6. Configure minizinc:

.. code-block:: bash

    cmake .. -DUSE_PROPRIETARY=on [-DCMAKE_PREFIX_PATH=/home/user/local/scip/installation] 

The optional prefix path variable is only necessary if you installed SCIP in a non-systemwide directory.

7. Compile Minizinc and enjoy SCIP as a solver.

If you have folders for SCIP and SoPlex separately, follow these steps.

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


Installation of MIP Backends: *COIN-OR CBC*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(CBC as of 2.10/stable. Prefer stable or even trunk).

**UNIX / Linux**:

.. code-block:: bash

  $ svn checkout https://projects.coin-or.org/svn/Cbc/stable/2.10/ Cbc-stable
  $ cd Cbc-stable
  $ ./configure <--enable-cbc-parallel>
  $ make && make install
  $ export CBC_HOME=$(pwd)               ## put this into .profile with $(pwd) expanded
                                         ## Or use -DOSICBC_ROOT=<absolute path> for MZN's CMake config

**Windows**, especially if you want the parallel version of CBC. Thanks to David Catteeuw.

1. OBTAIN AND BUILD CBC

  CBC is on github, but has many dependencies: https://github.com/coin-or/Cbc

  AMPL provides a CBC binary and has the entire project with dependencies on github: https://github.com/ampl/coin

  Building Couenne fails, but we don't need. Remove couenne, ipopt, and bonmin directories.
  
  Install zlib and bzip2, e.g., using vcpkg. Its CMake integration does not work as of vcpkg 2018.11.23 so you need to use its install folder manually,
  e.g., by adding the following to CBC's CMakeLists.txt:
  
    ::
     
       include_directories(...vcpkg/installed/x86-windows/include)
       link_libraries(...vcpkg/installed/x86-windows/lib/zlib.lib)
       link_libraries(...vcpkg/installed/x86-windows/lib/bz2.lib)
   
  Then 

  .. code-block:: bash
  
    $ cd C:\dev
    $ git clone https://github.com/ampl/coin.git
    $ cd coin
    $ rmdir /s Bonmin
    $ rmdir /s Couenne
    $ rmdir /s Ipopt
    $ mkdir build
    $ cd build
    $ cmake .. -G "Visual Studio 15 2017" -A x64 -DCMAKE_INSTALL_PREFIX=C:\dev\Cbc_install
    $ cmake --build . --target install
 
  => successfully builds ``Cbc.lib``, debug version in ``C:\dev\coin\build\Debug``.


2. BUILD MINIZINC WITH CBC

  Copy ``C:\dev\coin\build\Debug\*.lib`` to ``C:\dev\Cbc_install\lib``

  Configure MiniZinc's CMake with ``-DOSICBC_ROOT=C:\dev\cbc_install``.


Useful Flattening Parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following parameters can be given on the command line or modified in ``share/minizinc/linear/options.mzn``:

::

  -D nSECcuts=0/1/2                            %% Subtour Elimination Constraints, see below
  -D fMIPdomains=true/false                    %% The unified domains feature, see below
  -D float_EPS=1e-6                            %% Epsilon for floats' strict comparison
  -DfIndConstr=true -DfMIPdomains=false        %% Use solver's indicator constraints, see below

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
                     write model to <file> (.lp, .mps, .sav, ...)
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

By default it is off.
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

For general information of warm start annotations, see Tutorial.
Warm starts are currently implemented for Gurobi, IBM ILOG CPLEX, and XPRESS.


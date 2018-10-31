.. _ch-cmdline:

The MiniZinc Command Line Tool
==============================

The core of the MiniZinc constraint modelling system is the ``minizinc`` tool. You can use it directly from the command line, through the MiniZinc IDE, or through a programmatic interface (API). This chapter summarises the options accepted by the tool, and explains how it interacts with target solvers.

Basic Usage
-----------

The ``minizinc`` tool performs three basic functions: it *compiles* a MiniZinc model (plus instance data), it *runs* an external solver, and it *translates solver output* into the form specified in the model. Most users would use all three functions at the same time. For example, let us assume that we want to solve the following simple problem, given as two files (:download:`model.mzn <examples/command_line/model.mzn>` and :download:`data.dzn <examples/command_line/data.dzn>`):

.. literalinclude:: examples/command_line/model.mzn
  :language: minizinc

.. literalinclude:: examples/command_line/data.dzn
  :language: minizinc

To run the model file ``model.mzn`` with data file ``data.dzn`` using the Gecode solver, you can use the following command line:

.. code-block:: bash

  $ minizinc --solver Gecode model.mzn data.dzn

This would result in the output

.. code-block:: none

  The resulting values are [10, 9, 8, 7, 6].
  ----------
  ==========

However, each of the three functions can also be accessed individually. For example, to compile the same model and data, use the ``-c`` option:

.. code-block:: bash

  $ minizinc -c --solver Gecode model.mzn data.dzn

This will result in two new files, ``model.fzn`` and ``model.ozn``, being output in the same directory as ``model.mzn``. You could then run a target solver directly on the ``model.fzn`` file, or use ``minizinc``:

.. code-block:: bash

  $ minizinc --solver Gecode model.fzn

You will see that the solver produces output in a standardised form, but not the output prescribed by the output item in the model:

.. code-block:: none

  x = array1d(1..5 ,[10, 9, 8, 7, 6]);
  ----------
  ==========

The translation from this output to the form specified in the model is encoded in the ``model.ozn`` file. You can use ``minizinc`` to execute the ``.ozn`` file. In this mode, it reads a stream of solutions from standard input, so we need to pipe the solver output into ``minizinc``:

.. code-block:: bash

  $ minizinc --solver Gecode model.fzn | minizinc --ozn-file model.ozn

These are the most basic command line options that you need in order to compile and run models and translate their output. The next section lists all available command line options in detail. :numref:`sec-cmdline-conffiles` explains how new solvers can be added to the system.

Adding Solvers
--------------

Solvers that support MiniZinc typically consist of two parts: a solver *executable*, which can be run on the FlatZinc output of the MiniZinc compiler, and a *solver library*, which consists of a set of MiniZinc files that define the constraints that the solver supports natively. This section deals with making existing solvers available to the MiniZinc tool chain. For information on how to add FlatZinc support to a solver, refer to :numref:`ch-fzn-interfacing`.

Configuration files
~~~~~~~~~~~~~~~~~~~

In order for MiniZinc to be able to find both the solver library and the executable, the solver needs to be described in a *solver configuration file* (see :numref:`sec-cmdline-conffiles` for details). If the solver you want to install comes with a configuration file (which has the file extension ``.msc`` for MiniZinc Solver Configuration), it has to be in one of the following locations:

- In the ``minizinc/solvers/`` directory of the MiniZinc installation. If you install MiniZinc from the binary distribution, this directory can be found at ``/usr/share/minizinc/solvers`` on Linux systems, inside the MiniZincIDE application on macOS system, and in the ``Program Files\\MiniZinc IDE (bundled)`` folder on Windows.
- In the directory ``$HOME/.minizinc/solvers`` on Linux and macOS systems, and the Application Data directory on Windows systems.
- In any directory listed on the ``MZN_SOLVER_PATH`` environment variable (directories are separated by ``:`` on Linux and macOS, and by ``;`` on Windows systems).
- In any directory listed in the ``mzn_solver_path`` option of the global or user-specific configuration file (see :numref:`ch-user-config`)
- Alternatively, you can use the MiniZinc IDE to create solver configuration files, see :numref:`sec-ide-add-solvers` for details.

After adding a solver, it will be listed in the output of the ``minizinc --solvers`` command.

Configuration for MIP solvers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some solvers require additional configuration flags before they can be used. For example, the binary bundle of MiniZinc comes with interfaces to the CPLEX and Gurobi Mixed Integer Programming solvers. However, due to licensing restrictions, the solvers themselves are not part of the bundled release. Depending on where CPLEX or Gurobi is installed on your system, MiniZinc may be able to find the solvers automatically, or it may require an additional option to point it to the shared library.

In case the libraries cannot be found automatically, you can use one of the following:

- CPLEX: Specify the location of the shared library using the ``--cplex-dll`` command line option. On Windows, the library is called ``cplexXXXX.dll`` and typically found in same directory as the ``cplex`` executable. On Linux it is ``libcplexXXX.so``, and on macOS ``libcplexXXXX.jnilib``, where ``XXX`` and ``XXXX`` stand for the version number.
- Gurobi: The command line option for Gurobi is ``--gurobi-dll``. On Windows, the library is called ``gurobiXX.dll`` (in the same directory as the ``gurobi`` executable), and on Linux and macOS is it ``libgurobiXX.so`` (in the ``lib`` directory of your Gurobi installation).
- You can define these paths as defaults in your user configuration file, see :numref:`ch-user-config`.

.. _ch-cmdline-options:

Options
-------

You can get a list of all the options supported by the ``minizinc`` tool using the ``--help`` flag.

General options
~~~~~~~~~~~~~~~

These options control the general behaviour of the ``minizinc`` tool.

.. option::  --help, -h

    Print a help message.

.. option::  --version

    Print version information.

.. option::  --solvers

    Print list of available solvers.

.. option::  --solver <id>, --solver <solver configuration file>.msc

    Select solver to use. The first form of the command selects one of the
    solvers known to MiniZinc (that appear in the list of the ``--solvers``
    command). You can select a solver by name, id, or tag, and add a specific
    version. For example, to select a mixed-integer programming solver, 
    identified by the ``mip`` tag, you can use ``--solver mip``. To select
    a specific version of Gurobi (in case you have two versions installed),
    use ``--solver Gurobi@7.5.2``. Instead of the name you can also use
    the solver's identifier, e.g. ``--solver org.gecode.gecode``.
    
    The second form of the command selects the solver from the given
    configuration file (see :numref:`sec-cmdline-conffiles`).

.. option::  --help <id>

    Print help for a particular solver. The scheme for selecting a solver
    is the same as for the ``--solver`` option.

.. option::  -v, -l, --verbose

    Print progress/log statements (for both compilation and solver).
    Note that some solvers may log to stdout.

.. option::  --verbose-compilation

    Print progress/log statements for compilation only.

.. option::  -s, --statistics

    Print statistics (for both compilation and solving).

.. option::  --compiler-statistics

    Print statistics for compilation.

.. option::  -c, --compile

    Compile only (do not run solver).

.. option::  --config-dirs

    Output configuration directories.

.. option::  --solvers-json

    Print configurations of available solvers as a JSON array.

Solving options
~~~~~~~~~~~~~~~

Each solver may support specific command line options for controlling its behaviour. These can be queried using the ``--help <id>`` flag, where ``<id>`` is the name or identifier of a particular solver. Most solvers will support some or all of the following options.


.. option:: -a, --all-solutions

  Report *all* solutions in the case of satisfaction
  problems, or print *intermediate* solutions of increasing quality in the case
  of optimisation problems.

.. option:: -n <i>, --num-solutions <i>

  Stop after reporting ``i`` solutions (only used with satisfaction problems).

.. option:: -f, --free-search

  Instructs the solver to conduct a "free search", i.e., ignore any search 
  annotations. The solver is not *required* to ignore the annotations, but it
  is *allowed* to do so.

.. option:: --solver-statistics

  Print statistics during and/or after the search for solutions.

.. option:: --verbose-solving

  Print log messages (verbose solving) to the standard error stream.

.. option:: -p <i>, --parallel <i>

  Run with ``i`` parallel threads (for multi-threded solvers).

.. option:: -r <i>, --random-seed <i>

  Use ``i`` as the random seed (for any random number generators the solver
  may be using).

Flattener input options
~~~~~~~~~~~~~~~~~~~~~~~

These options control aspects of the MiniZinc compiler.

.. option::  --ignore-stdlib

    Ignore the standard libraries stdlib.mzn and builtins.mzn

.. option::  --instance-check-only

    Check the model instance (including data) for errors, but do not
    convert to FlatZinc.

.. option::  -e, --model-check-only

    Check the model (without requiring data) for errors, but do not
    convert to FlatZinc.

.. option::  --model-interface-only

    Only extract parameters and output variables.

.. option::  --model-types-only

    Only output variable (enum) type information.

.. option::  --no-optimize

    Do not optimize the FlatZinc

.. option::  -d <file>, --data <file>

    File named <file> contains data used by the model.

.. option::  -D <data>, --cmdline-data <data>

    Include the given data assignment in the model.

.. option::  --stdlib-dir <dir>

    Path to MiniZinc standard library directory

.. option::  -G --globals-dir --mzn-globals-dir <dir>

    Search for included globals in <stdlib>/<dir>.

.. option::  - --input-from-stdin

    Read problem from standard input

.. option::  -I --search-dir

    Additionally search for included files in <dir>.

.. option::  -D "fMIPdomains=false"

    No domain unification for MIP

.. option::  --MIPDMaxIntvEE <n>

    Max integer domain subinterval length to enforce equality encoding, default 0

.. option::  --MIPDMaxDensEE <n>

    Max domain cardinality to N subintervals ratio
    to enforce equality encoding, default 0, either condition triggers

.. option::  --only-range-domains

    When no MIPdomains: all domains contiguous, holes replaced by inequalities

.. option::  --allow-multiple-assignments

    Allow multiple assignments to the same variable (e.g. in dzn)

.. option::  --compile-solution-checker <file>.mzc.mzn

    Compile solution checker model.

Flattener two-pass options
++++++++++++++++++++++++++

Two-pass compilation means that the MiniZinc compiler will first compile the model in order to collect some global information about it, which it can then use in a second pass to improve the resulting FlatZinc. For some combinations of model and target solver, this can lead to substantial improvements in solving time. However, the additional time spent on the first compilation pass does not always pay off.

.. option::  --two-pass

    Flatten twice to make better flattening decisions for the target

.. option::  --use-gecode

    Perform root-node-propagation with Gecode (adds --two-pass)

.. option::  --shave

    Probe bounds of all variables at the root node (adds --use-gecode)

.. option::  --sac

    Probe values of all variables at the root node (adds --use-gecode)

.. option::  --pre-passes <n>

    Number of times to apply shave/sac pass (0 = fixed-point, 1 = default)

.. option::  -O<n>

    Two-pass optimisation levels:
    
    -O0:    Disable optimize (--no-optimize)
    -O1:    Single pass (default)
    -O2:    Same as: --two-pass              
    -O3:    Same as: --use-gecode
    -O4:    Same as: --shave                  
    -O5:    Same as: --sac

Flattener output options
++++++++++++++++++++++++

These options control how the MiniZinc compiler produces the resulting FlatZinc output. If you run the solver directly through the ``minizinc`` command or the MiniZinc IDE, you do not need to use any of these options.

.. option::  --no-output-ozn, -O-

    Do not output ozn file

.. option::  --output-base <name>

    Base name for output files

.. option::  --fzn <file>, --output-fzn-to-file <file>

    Filename for generated FlatZinc output

.. option::  -O, --ozn, --output-ozn-to-file <file>

    Filename for model output specification (-O- for none)

.. option::  --keep-paths

    Don't remove path annotations from FlatZinc

.. option::  --output-paths

    Output a symbol table (.paths file)

.. option::  --output-paths-to-file <file>

    Output a symbol table (.paths file) to <file>

.. option::  --output-to-stdout, --output-fzn-to-stdout

    Print generated FlatZinc to standard output

.. option::  --output-ozn-to-stdout

    Print model output specification to standard output

.. option::  --output-paths-to-stdout

    Output symbol table to standard output

.. option::  --output-mode <item|dzn|json>

    Create output according to output item (default), or output compatible
    with dzn or json format

.. option::  --output-objective

    Print value of objective function in dzn or json output

.. option::  -Werror

    Turn warnings into errors

Solution output options
~~~~~~~~~~~~~~~~~~~~~~~

These options control how solutions are output. Some of these options only apply if ``minizinc`` is used to translate a stream of solutions coming from a solver into readable output (using a .ozn file generated by the compiler).

.. option::  --ozn-file <file>

    Read output specification from ozn file.

.. option::  -o <file>, --output-to-file <file>

    Filename for generated output.

.. option::  -i <n>, --ignore-lines <n>, --ignore-leading-lines <n>

    Ignore the first <n> lines in the FlatZinc solution stream.

.. option::  --soln-sep <s>, --soln-separator <s>, --solution-separator <s>

    Specify the string printed after each solution (as a separate line).
    The default is to use the same as FlatZinc, "----------".

.. option::  --soln-comma <s>, --solution-comma <s>

    Specify the string used to separate solutions.
    The default is the empty string.

.. option::  --unsat-msg (--unsatisfiable-msg)

    Specify status message for unsatisfiable problems
    (default: ``"=====UNSATISFIABLE====="``)
    
.. option:: --unbounded-msg

    Specify status message for unbounded problems
    (default: ``"=====UNBOUNDED====="``)

.. option:: --unsatorunbnd-msg

    Specify status message for unsatisfiable or unbounded problems
    (default: ``"=====UNSATorUNBOUNDED====="``)

.. option:: --unknown-msg

    Specify status message if search finished before determining status
    (default: ``"=====UNKNOWN====="``)

.. option:: --error-msg

    Specify status message if search resulted in an error
    (default: ``"=====ERROR====="``)

.. option:: --search-complete-msg <msg>

    Specify status message if when search exhausted the entire search space
    (default: ``"=========="``)

.. option::  --non-unique

    Allow duplicate solutions.

.. option::  -c, --canonicalize

    Canonicalize the output solution stream (i.e., buffer and sort).

.. option::  --output-non-canonical <file>

    Non-buffered solution output file in case of canonicalization.

.. option::  --output-raw <file>

    File to dump the solver's raw output (not for hard-linked solvers)

.. option::  --no-output-comments

    Do not print comments in the FlatZinc solution stream.

.. option::  --output-time

    Print timing information in the FlatZinc solution stream.

.. option::  --no-flush-output

    Don't flush output stream after every line.

.. _ch-user-config:

User Configuration Files
------------------------

The ``minizinc`` tool reads a system-wide and a user-specific configuration file to determine default paths, solvers and solver options. The files are called ``Preferences.json``, and you can find out the locations for your platform using the option ``--config-dirs``:

.. code-block:: bash

  $ minizinc --config-dirs
  {
    "globalConfigFile" : "/Applications/MiniZincIDE.app/Contents/Resources/share/minizinc/Preferences.json",
    "userConfigFile" : "/Users/Joe/.minizinc/Preferences.json",
    "userSolverConfigDir" : "/Users/Joe/.minizinc/solvers",
    "mznStdlibDir" : "/Applications/MiniZincIDE.app/Contents/Resources/share/minizinc"
  }

The configuration files are simple JSON files that can contain the following configuration options:

- ``mzn_solver_path`` (list of strings): Additional directories to scan for solver configuration files.
- ``mzn_lib_dir`` (string): Location of the MiniZinc standard library.
- ``tagDefaults`` (list of lists of strings): Each entry maps a tag to the default solver for that tag. For example, ``[["cp","org.chuffed.chuffed"],["mip","org.minizinc.gurobi"]]`` would declare that whenever a solver with tag ``"cp"`` is requested, Chuffed should be used, and for the ``"mip"`` tag, Gurobi is the default. The empty tag (``""``) can
be used to define the system-wide default solver (i.e., the solver that is
chosen when running ``minizinc`` without the ``--solver`` argument).
- ``solverDefaults`` (list of lists of strings): Each entry consists of a list of three strings: a solver identifier, a command line option, and a value for that command line option.For example, ``[["org.minizinc.gurobi","--gurobi-dll", "/Library/gurobi752/mac64/lib/libgurobi75.so"]]`` would specify the Gurobi shared library to use (on a macOS system with Gurobi 7.5.2). For command line options that don't take a value, you have to specify an empty string, e.g. ``[["org.minizinc.gurobi","--uniform-search",""]]``.

Here is a sample configuration file:

.. code-block:: json

  {
    "mzn_solver_path": ["/usr/share/choco"],
    "tagDefaults": [["cp","org.choco-solver.choco"],["mip","org.minizinc.cplex"],["","org.gecode.gecode"]],
    "solverDefaults": [["org.minizinc.cplex","--cplex-dll","/opt/CPLEX_Studio128/cplex/bin/x86-64_sles10_4.1/libcplex128.so"]] 
  }

Configuration values in the user-specific configuration file override the global values, except for solver default arguments, which are only overridden if the name of the option is the same, and otherwise get added to the command line.

Note: Due to current limitations in MiniZinc's JSON parser, we use lists of strings rather than objects for the default mappings. This may change in a future release, but the current syntax will remain valid. The location of the global configuration is currently the ``share/minizinc`` directory of the MiniZinc installation. This may change in future versions to be more consistent with file system standards (e.g., to use ``/etc`` on Linux and ``/Library/Preferences`` on macOS).

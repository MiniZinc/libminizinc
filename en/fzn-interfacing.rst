.. _ch-fzn-interfacing:

Interfacing Solvers to Flatzinc
===============================

.. _sec-cmdline-conffiles:

Solver Configuration Files
--------------------------

In order for a solver to be available to MiniZinc, it has to be described in a *solver configuration file*. This is a simple file, in JSON or ``.dzn`` format, that contains some basic information such as the solver's name, version, where its library of global constraints can be found, and a path to its executable.

A solver configuration file must have file extension ``.msc`` (for MiniZinc Solver Configuration), and can be placed in any of the following locations:

- In the ``minizinc/solvers/`` directory of the MiniZinc installation. If you install MiniZinc from the binary distribution, this directory can be found at ``/usr/share/minizinc/solvers`` on Linux systems, inside the MiniZincIDE application on macOS system, and in the ``Program Files\\MiniZinc IDE (bundled)`` folder on Windows.
- In the directory ``$HOME/.minizinc/solvers`` on Linux and macOS systems, and the Application Data directory on Windows systems.
- In any directory listed on the ``MZN_SOLVER_PATH`` environment variable (directories are separated by ``:`` on Linux and macOS, and by ``;`` on Windows systems).
- In any directory listed in the ``mzn_solver_path`` option of the global or user-specific configuration file (see :numref:`ch-user-config`)
- Alternatively, you can use the MiniZinc IDE to create solver configuration files, see :numref:`sec-ide-add-solvers` for details.

Solver configuration files must be valid JSON or ``.dzn`` files. As a JSON file, it must be an object with certain fields. As a ``.dzn`` file, it must consist of assignment items.

For example, a simple solver configuration in JSON format could look like this:

.. code-block:: json

  {
    "name" : "My Solver",
    "version": "1.0",
    "id": "org.myorg.my_solver",
    "executable": "fzn-mysolver"
  }


The same configuration in ``.dzn`` format would look like this:

.. code-block:: minizinc

  name = "My Solver";
  version = "1.0";
  id = "org.myorg.my_solver";
  executable = "fzn-mysolver";

Here is a list of all configuration options recognised by the configuration file parser. Any valid configuration file must at least contain the fields ``name``, ``version``, ``id``, and ``executable``.

- ``name`` (string, required): The name of the solver (displayed, together with the version, when you call ``minizinc --solvers``, and in the MiniZinc IDE).
- ``version`` (string, required): The version of the solver.
- ``id`` (string, required): A unique identifier for the solver, "reverse domain name" notation.
- ``executable`` (string, required): The executable for this solver that can run FlatZinc files. This can be just a file name (in which case the solver has to be on the current PATH), or an absolute path to the executable, or a relative path (which is interpreted relative to the location of the configuration file).
- ``mznlib`` (string, default ``""``): The solver-specific library of global constraints and redefinitions. This should be the name of a directory (either an absolute path or a relative path, interpreted relative to the location of the configuration file). For solvers whose libraries are installed in the same location as the MiniZinc standard library, this can also take the form ``-G<solverlib>``, e.g., ``-Ggecode`` (this is mostly the case for solvers that ship with the MiniZinc binary distribution).
- ``tags`` (list of strings, default empty): Each solver can have one or more tags that describe its features in an abstract way. Tags can be used for selecting a solver using the ``--solver`` option. There is no fixed list of tags, however we recommend using the following tags if they match the solver's behaviour:

  - ``"cp"``: for Constraint Programming solvers
  - ``"mip"``: for Mixed Integer Programming solvers
  - ``"float"``: for solvers that support float variables
  - ``"api"``: for solvers that use the internal C++ API

- ``stdFlags`` (list of strings, default empty): Which of the standard solver command line flags are supported by this solver. The standard flags are:

  - ``-a``: Print all intermediate solutions for optimisation problems, or all solutions for satisfaction problems.
  - ``-n``: Stop search after how many solutions (with integer argument).
  - ``-s``: Produce statistics output during and/or after solving.
  - ``-p``: How many parallel threads to run (with integer argument).
  - ``-r``: Initial seed for random numbers (with integer argument).
  - ``-f``: Use "free search" mode (where the solver is allowed to ignore any search annotations).

- ``extraFlags`` (list of list of strings, default empty): Extra command line flags supported by the solver. Each entry should be a list two, three or four strings. The first string is the name of the option (e.g. ``"--special-algorithm"``). The second string is a description that can be used to generate help output (e.g. ``"which special algorithm to use"``). The third string specifies the type of the argument (as a MiniZinc type). The fourth string is the default value. If no type is specified, ``"bool"`` is assumed.
- ``supportsMzn`` (bool, default ``false``): Whether the solver can run MiniZinc directly (i.e., it implements its own compilation or interpretation of the model).
- ``supportsFzn`` (bool, default ``true``): Whether the solver can run FlatZinc. This should be the case for most solvers
- ``needsSolns2Out`` (bool, default ``true``): Whether the output of the solver needs to be passed through the MiniZinc output processor.
- ``needsMznExecutable`` (bool, default ``false``): Whether the solver needs to know the location of the MiniZinc executable. If true, it will be passed to the solver using the ``mzn-executable`` option.
- ``needsStdlibDir`` (bool, default ``false``): Whether the solver needs to know the location of the MiniZinc standard library directory. If true, it will be passed to the solver using the ``stdlib-dir`` option.
- ``isGUIApplication`` (bool, default ``false``): Whether the solver has its own graphical user interface, which means that MiniZinc will detach from the process and not wait for it to finish or to produce any output.


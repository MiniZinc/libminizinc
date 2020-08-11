<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://www.minizinc.org/">
    <img src="https://www.minizinc.org/MiniZn_logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">MiniZinc</h3>

  <p align="center">
    A high-level constraint modelling language that allows you to easily
    express and solve discrete optimisation problems.
    <br />
    <a href="https://www.minizinc.org/"><strong>Visit our website »</strong></a>
    <br />
    <br />
    <a href="https://www.minizinc.org/doc-latest/">View Documentation</a>
    ·
    <a href="https://github.com/MiniZinc/libminizinc/issues">Report Bug</a>
    ·
    <a href="https://github.com/MiniZinc/libminizinc/issues">Request Feature</a>
  </p>
</p>


<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
* [Getting Started](#getting-started)
  * [Installation](#installation)
  * [Usage](#usage)
* [Building](#building)
  * [Prerequisites](#prerequisites)
  * [Compilation](#compilation)
* [Testing](#testing)
* [License](#license)
* [Contact](#contact)


<!-- ABOUT THE PROJECT -->
## About The Project

MiniZinc is a free and open-source constraint modeling language.

You can use MiniZinc to model constraint satisfaction and optimisation problems
in a high-level, solver-independent way, taking advantage of a large library of
pre-defined constraints. Your model is then compiled into FlatZinc, a solver
input language that is understood by a wide range of solvers.

MiniZinc is developed at Monash University in collaboration with Data61
Decision Sciences.

<!-- GETTING STARTED -->
## Getting Started

To get a MiniZinc up and running follow these simple steps.

### Installation

The recommended way to install _MiniZinc_ is by the use of the bundled binary
packages. These packages are available for machines running Linux, Mac, and
Windows.

The latest release can be found on [the MiniZinc
website](http://www.minizinc.org/software.html).

### Usage

Once the MiniZinc bundle is installed on your machine, you can start expressing
and solving discrete optimisation problems.  The following code segment shows a
MiniZinc model for the well known n-queens problem.

```minizinc
int: n = 8; % The number of queens.

array [1..n] of var 1..n: q;

include "alldifferent.mzn";

constraint alldifferent(q);
constraint alldifferent(i in 1..n)(q[i] + i);
constraint alldifferent(i in 1..n)(q[i] - i);
```

You have two easy options to solve this model:
 - In the MiniZincIDE: Select your preferred solver and press the "Run" button.
 - With the `minizinc` executable available on your path: run `minizinc --solver gecode nqueens.mzn`.

_For more example MiniZinc models and more information about working with
MiniZinc, please refer to our
[Documentation](https://www.minizinc.org/doc-latest/)_

<!-- BUILDING INSTRUCTIONS -->
## Building

The following instructions will help you compile the MiniZinc compiler. Note
that this repository does not include the IDE, findMUS, or any solvers that are
part of the MiniZinc project. These can be found in the following repositories:

 - [MiniZincIDE](https://github.com/MiniZinc/MiniZincIDE)
 - [Gecode](https://github.com/Gecode/gecode)
 - [Chuffed](https://github.com/chuffed/chuffed)

### Prerequisites

- [CMake](https://cmake.org/) (>=3.4)
- A recent C++ compiler - Compilation is tested with recent versions of Clang,
  GCC, and Microsoft Visual C++.
- (optional) [Bison](https://www.gnu.org/software/bison/) (>=3.4) and
  [Flex](https://github.com/westes/flex) (>=2.5) - To make changes to the
  MiniZinc lexer or parser.
- (optional) [Gecode](https://www.gecode.org/) - To compile the internal Gecode
  solver interface (included in the MiniZinc bundle)
- (optional) [Coin OR's CBC](https://www.coin-or.org/) - To compile the
  internal CBC solver interface (included in the MiniZinc bundle)
- (optional) Proprietary solver headers
  ([CPLEX](https://www.ibm.com/analytics/cplex-optimizer),
  [Gurobi](https://www.gurobi.com/), [SCIP](https://www.scipopt.org/),
  [Xpress](https://www.fico.com/)) - To load these solvers at runtime (included
  in the MiniZinc bundle)

### Compilation

The MiniZinc compiler is compiled as a CMake project. CMake's [User Interaction
Guide](https://cmake.org/cmake/help/latest/guide/user-interaction/index.html)
can provide you with a quick introduction to compiling CMake projects. The
following CMake variables can be used in the MiniZinc project to instruct the
compilation behaviour:

| Variable                                     | Default | Description                                                 |
|----------------------------------------------|---------|-------------------------------------------------------------|
| CMAKE_BUILD_TYPE                             | Release | Build type of single-configuration generators.              |
| CMAKE_INSTALL_PREFIX                         |         | Install directory used by `--target install`.               |
| CMAKE_POSITION_INDEPENDENT_CODE              | TRUE    | Whether to create a position-independent targets            |
| **<solver_name>**_ROOT                       |         | Additional directory to look for **<solver_name>**          |
| CMAKE_DISABLE_FIND_PACKAGE_**<solver_name>** | FALSE   | Disable compilation of **<solver_name>**'s solver interface |
| USE_PROPRIETARY                              | FALSE   | Allow static linking of proprietary solvers                 |
| **<Gurobi/CPlex>**_PLUGIN                    | TRUE    | Load solver at runtime (instead of static compilation)      |

Possible values for **<solver_name>** are `CPlex`, `Geas`, `Gecode`, `Gurobi`,
`OsiCBC`, `SCIP`, and `Xpress`.

<!-- TESTING INSTRUCTIONS -->
## Testing

The correctness of the MiniZinc compiler is tested using a
[PyTest](https://docs.pytest.org/en/stable/) test suite. Instruction on how to
run the test suite and how to add new tests can be found
[here](https://github.com/MiniZinc/libminizinc/tree/master/tests)


<!-- LICENSE -->
## License

Distributed under the Mozilla Public License Version 2.0. See `LICENSE` for more information.


<!-- CONTACT -->
## Contact

🏛 **MiniZinc Community**
  - Website: [https://www.minizinc.org/](https://www.minizinc.org/)
  - StackOverflow: [https://stackoverflow.com/questions/tagged/minizinc](https://stackoverflow.com/questions/tagged/minizinc)
  - Google Groups: [https://groups.google.com/g/minizinc](https://groups.google.com/g/minizinc)

🏛 **Monash Optimisation Group**
  - Website: [https://www.monash.edu/it/dsai/optimisation](https://www.monash.edu/it/dsai/optimisation)

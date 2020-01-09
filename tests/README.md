MiniZinc Testing
================

## Setup

Requires Python 3 (on Windows 3.8 is required). Make sure you're in the `tests/` directory.

```sh
pip install -r requirements.txt
```

## Running Test Suite

To run the full test suite:

```sh
pytest
```

An HTML report will be generated at `output/report.html`.

## Multiple test suites

To facilitate running the test suite with different minizinc options, `specs/suites.yml` contains configurations for running tests, or a subset of tests using different options.

```yaml
my-test-suite: !Suite
  includes: ['*'] # Globs for included .mzn files
  solvers: [gecode, chuffed] # The allowed solvers (if the test case itself specifies a different solver it will be skipped)
  strict: false # Allow tests to pass if they check against another solver (default true)
  options:
    -O3: true # Default options to pass to minizinc (merged and overwritten by individual test cases)
```

For example, to run the `optimize-2` and `no-mip-domains` configurations only:

```sh
pytest --suite optimize-2 --suite no-mip-domains
```

## Creating/editing test cases with the web interface

A web interface can be used to create and edit test cases graphically. Python 3.8.1 or newer and Flask is required.

```sh
pip install flask
```

Start the web interface with

```sh
cd tests
python -m minizinc_testing.create
```

Then open `http://localhost:5000`.

The web interface detects `.mzn` files in the `tests/spec` directory. To add a new test, simply create a `.mzn` containing the model, and then open the file in the web interface. Test cases can then be generated accordingly.

## Creating/editing test cases manually

The test cases are defined using `YAML` inside the minizinc `.mzn` files. This YAML definition must be inside a block comment like the following:

```c
/***
!Test
expected: !Result
  solution:
    x: 1
***/
```

Multiple cases can be specified for one `.mzn` file:

```c
/***
--- !Test
  ...
--- !Test
  ...
--- !Test
  ...
***/
```

### YAML format

The format of the test case spec is as follows:

```yaml
!Test
solvers: [gecode, cbc, chuffed] # List of solvers to use (omit if all solvers should be tested)
check_against: [gecode, cbc, chuffed] # List of solvers used to check results (omit if no checking is needed)
extra_files: [datafile.dzn] # Data files to use if any
options: # Options passed to minizinc-python's solve(), usually all_solutions if present
  all_solutions: true
  timeout: !Duration 10s
expected: # The obtained result must match one of these
- !Result
  status: SATISFIED # Result status
  solution: !Solution
    s: 1
    t: !!set {1, 2, 3} # The set containing 1, 2 and 3
    u: !Range 1..10 # The range 1 to 10 (inclusive)
    v: [1, 2, 3] # The array with 1, 2, 3
    x: !Unordered [3, 2, 1] # Ignore the order of elements in this array
    _output_item: !Trim |
      trimmed output item
      gets leading/trailing
      whitespace ignored 
- !Error
  type: MiniZincError # Name of the error type
```

For a test to pass, at least one expected result must be a subset of the obtained result. That is, the obtained result can have more attributes, but not less, and corresponding attributes must match.

If a solution is produced that does not match any given expected output, the result is checked using another solver. If this check passes then the test passes with a warning.

### Multiple solutions

When setting `all_solutions: true` and the order of the returned solutions often does not matter, use `!SolutionSet` for the list of solutions:

```yaml
!Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: 1
  - !Solution
    x: 2
```

### Testing FlatZinc output

Use `type: compile` on a test to enable only flattening.
Then `!FlatZinc filename.fzn` to give files with expected results.

```yaml
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc expected.fzn
```

## TODO

- Tool for generating test cases
- Tweak YAML parsing so not so many !Tags are required
- Better documentation of how the framework operates
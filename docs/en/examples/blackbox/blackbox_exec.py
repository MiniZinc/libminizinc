#!/usr/bin/env python3
"""Template: MiniZinc black-box propagator, executable (subprocess) mode.

Use from MiniZinc:
    predicate my_prop(array[int] of var int: xs)
        ::minizinc_value_propagator ::blackbox_exec("python3", ["blackbox_exec.py"]);
    % or, if the file is executable and has the shebang above:
    %   ::blackbox_exec("./blackbox_exec.py");

The solver starts this program once and communicates over stdin/stdout, one
line per propagation:

    request : <int values, comma-separated>;<float values, comma-separated>\\n
    response: <int outputs, comma-separated>;<float outputs, comma-separated>\\n

A segment may be empty. For a BOUNDS propagator each segment holds two entries
per variable, a lower then an upper bound: "lb0,ub0,lb1,ub1;\\n".

The program must flush stdout after every response and keep looping until stdin
reaches end-of-file.
"""
import sys


def parse(segment, convert):
    return [convert(tok) for tok in segment.split(",") if tok.strip()]


def main():
    # The extra arguments from the ::blackbox_exec annotation are passed on the
    # command line, so they are available in sys.argv[1:].
    args = sys.argv[1:]  # noqa: F841 (unused in this example)

    for line in sys.stdin:
        int_seg, _, float_seg = line.partition(";")
        ints = parse(int_seg, int)
        floats = parse(float_seg, float)  # noqa: F841 (unused in this example)

        # Example (relational value propagator): accept (1) iff the integer
        # inputs sum to an even number, reject (0) otherwise.
        accept = 1 if sum(ints) % 2 == 0 else 0
        print(f"{accept};", flush=True)  # flush after each response


if __name__ == "__main__":
    main()

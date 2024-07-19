/*
 * Template: MiniZinc black-box propagator, executable (subprocess) mode.
 *
 * Build:  cc -o blackbox blackbox_exec.c
 *
 * Use from MiniZinc:
 *   predicate my_prop(array[int] of var int: xs)
 *       ::minizinc_value_propagator ::blackbox_exec("./blackbox");
 *   % extra command-line arguments can be passed as a second argument:
 *   %   ::blackbox_exec("./blackbox", ["--mode", "fast"]);
 *
 * The solver starts this program once (with the given arguments) and then
 * communicates over stdin/stdout, one line per propagation:
 *
 *   request : <int values, comma-separated>;<float values, comma-separated>\n
 *   response: <int outputs, comma-separated>;<float outputs, comma-separated>\n
 *
 * A segment may be empty (e.g. "5,-7;\n" carries no floats). For a BOUNDS
 * propagator each segment holds two entries per variable, a lower then an
 * upper bound: "lb0,ub0,lb1,ub1;\n".
 *
 * The program must flush stdout after every response and keep looping until
 * stdin reaches end-of-file, at which point it should exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  (void)argc; /* argv[1..] are the extra arguments from ::blackbox_exec */
  (void)argv;

  char* line = NULL;
  size_t cap = 0;
  while (getline(&line, &cap, stdin) != -1) {
    char* semi = strchr(line, ';');
    if (semi == NULL) {
      continue; /* malformed line */
    }
    *semi = '\0';
    char* int_seg = line;
    /* char *float_seg = semi + 1;  parse floats here if your propagator uses them */

    /* Example (relational value propagator): accept (1) iff the integer inputs
     * sum to an even number, reject (0) otherwise. */
    long long sum = 0;
    for (char* tok = strtok(int_seg, ","); tok != NULL; tok = strtok(NULL, ",")) {
      sum += atoll(tok);
    }

    printf("%d;\n", (sum % 2 == 0) ? 1 : 0);
    fflush(stdout); /* MUST flush after each response */
  }

  free(line);
  return 0;
}

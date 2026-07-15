import subprocess
import sys

from minizinc import default_driver, Driver

MODEL = 'var 1..3: x;\nconstraint x > 1;\nsolve satisfy;\noutput ["x = \\(x)\\n"];\n'

# A fake interactive FlatZinc solver: it reads commands from stdin and replies on
# stdout. On "solve" it emits a solution bracketed by the MiniZinc solution
# markers (so MiniZinc routes it through solns2out / the .ozn output item);
# everything else is echoed verbatim. Every reply is flushed immediately, so no
# stdio buffering hides it from the pipe transport. Written in Python so it runs
# on every platform (including Windows).
FAKE_SOLVER = r'''import sys
sys.stdout.write("ready\n")
sys.stdout.flush()
while True:
    line = sys.stdin.readline()
    if not line:
        break
    cmd = line.rstrip("\r\n")
    if cmd == "solve":
        for msg in ("thinking", "%%%mzn-sol-begin", "x = 2;", "----------",
                    "%%%mzn-sol-end", "done"):
            sys.stdout.write(msg + "\n")
        sys.stdout.flush()
    elif cmd == "quit":
        break
    else:
        sys.stdout.write("echo: " + cmd + "\n")
        sys.stdout.flush()
'''

# Like FAKE_SOLVER, but mimics a REPL that prints a prompt WITHOUT a trailing
# newline before reading. The begin marker therefore ends up glued to the prompt
# in the solver's stdout stream ("solver> %%%mzn-sol-begin"), which must still be
# recognised and stripped.
#
# The brief pause after the first prompt models a real REPL blocking for user
# input: it lets MiniZinc read (and eagerly echo) the prompt before the begin
# marker is produced, so the two arrive in separate reads. Without a genuine gap
# the OS may hand both to MiniZinc in a single read as one line, which is a test
# artifact of pre-filling stdin (a real interactive solver always waits for the
# user, draining the prompt first).
PROMPT_SOLVER = r'''import sys
import time
sys.stdout.write("solver> ")
sys.stdout.flush()
time.sleep(0.5)
while True:
    line = sys.stdin.readline()
    if not line:
        break
    cmd = line.rstrip("\r\n")
    if cmd == "solve":
        for msg in ("%%%mzn-sol-begin", "x = 2;", "----------", "%%%mzn-sol-end"):
            sys.stdout.write(msg + "\n")
        sys.stdout.write("solver> ")
        sys.stdout.flush()
    elif cmd == "quit":
        sys.stdout.write("bye\n")
        sys.stdout.flush()
        break
    else:
        sys.stdout.write("you said: " + cmd + "\n")
        sys.stdout.write("solver> ")
        sys.stdout.flush()
'''


def _run_interactive(tmp_path, script, stdin):
    assert isinstance(default_driver, Driver)
    solver = tmp_path / "fake_solver.py"
    solver.write_text(script)
    model = tmp_path / "model.mzn"
    model.write_text(MODEL)
    p = subprocess.run(
        [
            default_driver._executable,
            "--solver", "mzn-fzn",
            "--fzn-cmd", sys.executable,
            "--fzn-flag", str(solver),
            "--fzn-interactive",
            str(model),
        ],
        input=stdin,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert p.returncode == 0, (p.stdout.decode(), p.stderr.decode())
    return p.stdout.decode()


def test_fzn_interactive(tmp_path):
    """--fzn-interactive must pass the user's stdin to the solver and route only
    marker-delimited solution blocks through solns2out, echoing the rest verbatim."""
    out = _run_interactive(tmp_path, FAKE_SOLVER, b"hello\nsolve\nquit\n")

    # The piped command reached the solver's stdin (previously: instant EOF).
    assert "echo: hello" in out
    # Verbatim solver chatter passed straight through.
    assert "thinking" in out
    assert "done" in out
    # The marker block was mapped through the model's output item ("x = 2", no ';').
    assert "x = 2" in out
    # The raw FlatZinc assignment was consumed by solns2out, not echoed verbatim.
    assert "x = 2;" not in out
    # The markers themselves were consumed, not printed.
    assert "%%%mzn-sol-begin" not in out
    assert "%%%mzn-sol-end" not in out
    # No trailing search-status line is emitted for an interactive session.
    assert "=====" not in out


def test_fzn_interactive_prompt_glued_to_marker(tmp_path):
    """A no-newline prompt glues the begin marker onto the prompt line in the
    solver's stdout ("solver> %%%mzn-sol-begin"); the marker must still be
    recognised and stripped, and the solution mapped."""
    out = _run_interactive(tmp_path, PROMPT_SOLVER, b"solve\nquit\n")

    # The prompt is shown verbatim...
    assert "solver>" in out
    # ...but the markers glued to it are stripped...
    assert "%%%mzn-sol-begin" not in out
    assert "%%%mzn-sol-end" not in out
    # ...and the block is still mapped through the .ozn output item.
    assert "x = 2" in out
    assert "x = 2;" not in out
    assert "=====" not in out

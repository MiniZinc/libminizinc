import os
import signal
import subprocess
import sys
import time

from minizinc import Driver, default_driver

MODEL = "var 1..3: x;\nconstraint x > 1;\nsolve satisfy;\n"

# A fake FlatZinc solver that never terminates on its own. Given a PID file and a
# depth, every process in the tree appends its own PID to the file once, then the
# solver grows a chain of `depth` descendant processes by re-invoking its own
# script one level deeper. A tree of `depth` therefore records depth + 1 PIDs.
# The tests use these PIDs to confirm the *whole* solver process tree — not just
# the direct child — is terminated when MiniZinc times out or is interrupted.
#
# Descendants are started with subprocess (not os.fork), so this runs on every
# platform: on POSIX they are killed via the solver's process group, on Windows
# via MiniZinc's job object.
TREE_SOLVER = r'''
import os, signal, subprocess, sys, time

pidfile = sys.argv[1]
depth = int(sys.argv[2])
ignore_sigint = sys.argv[3] == "1"
if ignore_sigint:
    # Model a solver that does not shut down on Ctrl-C, so the test can check
    # that MiniZinc escalates to a stronger signal instead of hanging.
    try:
        signal.signal(signal.SIGINT, signal.SIG_IGN)
    except (ValueError, OSError):
        pass
with open(pidfile, "a") as f:
    f.write(str(os.getpid()) + "\n")
if depth > 0:
    subprocess.Popen(
        [sys.executable, sys.argv[0], pidfile, str(depth - 1), "1" if ignore_sigint else "0"],
        stdin=subprocess.DEVNULL,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
sys.stderr.write("solver up\n")
sys.stderr.flush()
while True:
    time.sleep(0.2)
'''


def _pid_alive(pid):
    if os.name == "posix":
        try:
            os.kill(pid, 0)
        except ProcessLookupError:
            return False
        except PermissionError:
            return True
        return True
    # Windows: os.kill is not a liveness check, so query the process directly.
    import ctypes

    PROCESS_QUERY_LIMITED_INFORMATION = 0x1000
    STILL_ACTIVE = 259
    kernel32 = ctypes.windll.kernel32
    handle = kernel32.OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, False, pid)
    if not handle:
        return False
    try:
        code = ctypes.c_ulong()
        if not kernel32.GetExitCodeProcess(handle, ctypes.byref(code)):
            return False
        return code.value == STILL_ACTIVE
    finally:
        kernel32.CloseHandle(handle)


def _read_pids(pidfile):
    try:
        return [int(line) for line in pidfile.read_text().split() if line.isdigit()]
    except OSError:
        return []


def _wait_for_pids(pidfile, expected, proc, timeout=20.0):
    """Block until the solver tree has recorded `expected` PIDs."""
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        pids = _read_pids(pidfile)
        if len(pids) >= expected:
            return pids
        if proc.poll() is not None:
            raise AssertionError("MiniZinc exited before the solver tree started")
        time.sleep(0.05)
    raise AssertionError(
        "solver tree never started {} processes (saw {})".format(
            expected, _read_pids(pidfile)
        )
    )


def _assert_all_dead(pids, timeout=10.0):
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if not any(_pid_alive(pid) for pid in pids):
            return
        time.sleep(0.05)
    alive = [pid for pid in pids if _pid_alive(pid)]
    raise AssertionError("solver processes survived teardown: {}".format(alive))


def _start(tmp_path, extra_args, depth, ignore_sigint=False):
    assert isinstance(default_driver, Driver)
    solver = tmp_path / "fake_solver.py"
    solver.write_text(TREE_SOLVER)
    pidfile = tmp_path / "pids"
    model = tmp_path / "model.mzn"
    model.write_text(MODEL)
    args = [
        default_driver._executable,
        "--solver",
        "mzn-fzn",
        "--fzn-cmd",
        sys.executable,
        "--fzn-flag",
        str(solver),
        "--fzn-flag",
        str(pidfile),
        "--fzn-flag",
        str(depth),
        "--fzn-flag",
        "1" if ignore_sigint else "0",
    ]
    args += list(extra_args)
    args.append(str(model))
    popen_kwargs = {"stdout": subprocess.PIPE, "stderr": subprocess.PIPE}
    if os.name == "nt":
        # MiniZinc tears down its solver by broadcasting a Ctrl-C to its console
        # (GenerateConsoleCtrlEvent, group 0). Give it its own (hidden) console so
        # that interrupt is not delivered to the test runner as well. This mirrors
        # what MiniZinc Python does when spawning the driver.
        popen_kwargs["creationflags"] = subprocess.CREATE_NEW_CONSOLE
        popen_kwargs["startupinfo"] = subprocess.STARTUPINFO(
            dwFlags=subprocess.STARTF_USESHOWWINDOW, wShowWindow=subprocess.SW_HIDE
        )
    return subprocess.Popen(args, **popen_kwargs), pidfile


def _interrupt(proc):
    """Interrupt a running MiniZinc process, the way an external tool would."""
    if os.name == "nt":
        # MiniZinc listens on a named pipe (used by e.g. the IDE and MiniZinc
        # Python) and turns a message into a Ctrl-C for its own console; see the
        # InterruptListener in include/minizinc/interrupt.hh.
        with open(f"\\\\.\\pipe\\minizinc-{proc.pid}", mode="w") as named_pipe:
            named_pipe.write("")
    else:
        proc.send_signal(signal.SIGINT)


def test_fzn_time_limit_terminates(tmp_path):
    """--time-limit must terminate a non-cooperative solver and return UNKNOWN
    without hanging."""
    p, _ = _start(tmp_path, ["--time-limit", "1000"], depth=0)
    try:
        out, _ = p.communicate(timeout=30)
    except subprocess.TimeoutExpired:
        p.kill()
        raise AssertionError("MiniZinc did not honour --time-limit (hung)")
    assert p.returncode == 0, p.returncode
    assert b"=====UNKNOWN=====" in out


def test_fzn_time_limit_kills_solver_tree(tmp_path):
    """The time limit must terminate the whole solver process tree (child *and*
    grandchild) — via the process group on POSIX and the job object on Windows."""
    p, pidfile = _start(tmp_path, ["--time-limit", "1000"], depth=1)
    pids = _wait_for_pids(pidfile, 2, p)
    try:
        p.communicate(timeout=30)
    except subprocess.TimeoutExpired:
        p.kill()
        raise AssertionError("MiniZinc did not honour --time-limit (hung)")
    assert p.returncode == 0, p.returncode
    _assert_all_dead(pids)


def test_fzn_time_limit_kills_signal_ignoring_solver(tmp_path):
    """A solver (and its children) that ignore Ctrl-C must still be terminated
    when the time limit is reached: on Windows via the job object, on POSIX via
    escalation to a stronger signal. This exercises the fallback teardown rather
    than the solver shutting itself down on the initial Ctrl-C."""
    p, pidfile = _start(tmp_path, ["--time-limit", "1000"], depth=1, ignore_sigint=True)
    pids = _wait_for_pids(pidfile, 2, p)
    try:
        p.communicate(timeout=30)
    except subprocess.TimeoutExpired:
        p.kill()
        raise AssertionError("MiniZinc did not honour --time-limit (hung)")
    assert p.returncode == 0, p.returncode
    _assert_all_dead(pids)


def test_fzn_interrupt_kills_solver_tree(tmp_path):
    """An interrupt mid-solve must terminate the whole solver process tree."""
    p, pidfile = _start(tmp_path, [], depth=1)
    pids = _wait_for_pids(pidfile, 2, p)
    assert all(_pid_alive(pid) for pid in pids)
    _interrupt(p)
    try:
        p.communicate(timeout=30)
    except subprocess.TimeoutExpired:
        p.kill()
        raise AssertionError("MiniZinc did not exit after interrupt (hung)")
    _assert_all_dead(pids)
    # On POSIX MiniZinc re-raises the interrupt, so it terminates via the signal.
    if os.name == "posix":
        assert p.returncode < 0, p.returncode


def test_fzn_interrupt_kills_signal_ignoring_solver(tmp_path):
    """A solver that ignores the interrupt must still be terminated: MiniZinc has
    to fall back to a stronger mechanism (SIGKILL escalation on POSIX, the job
    object on Windows) rather than block waiting for output that never comes.
    On POSIX this also guards against regressing the read-after-EINTR hang."""
    p, pidfile = _start(tmp_path, [], depth=0, ignore_sigint=True)
    pids = _wait_for_pids(pidfile, 1, p)
    _interrupt(p)
    try:
        p.communicate(timeout=30)
    except subprocess.TimeoutExpired:
        p.kill()
        raise AssertionError("MiniZinc hung on an interrupt-ignoring solver")
    _assert_all_dead(pids)
    if os.name == "posix":
        assert p.returncode < 0, p.returncode

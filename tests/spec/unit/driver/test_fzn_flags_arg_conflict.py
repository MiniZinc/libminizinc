from minizinc import default_driver, Driver
import subprocess


def test_fzn_flags_arg_conflict():
    assert isinstance(default_driver, Driver)
    # Should fail with no model file given, and not print help
    p = subprocess.run(
        [default_driver._executable, "--fzn-flags", "--help"],
        stdin=None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert p.returncode == 1
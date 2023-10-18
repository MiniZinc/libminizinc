from pathlib import Path
from minizinc import default_driver, Driver
import subprocess
import json


def test_command_line_error():
    assert isinstance(default_driver, Driver)
    p = subprocess.run(
        [default_driver._executable, "--made-up-argument", "--json-stream"],
        stdin=None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert p.returncode == 1
    messages = [json.loads(l) for l in p.stdout.splitlines(False)]
    errors = [m for m in messages if m["type"] == "error"]
    assert len(errors) == 1
    assert errors[0]["what"] == "argument parsing error"

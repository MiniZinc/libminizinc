from pathlib import Path
from minizinc import default_driver, Driver
import subprocess
import json


def test_run_fzn():
    here = Path(__file__).resolve().parent
    assert isinstance(default_driver, Driver)
    model_file = here / "test_run_fzn.fzn"
    p = subprocess.run(
        [default_driver._executable, model_file, "--json-stream"],
        stdin=None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert p.returncode == 0
    messages = [json.loads(l) for l in p.stdout.splitlines(False)]
    assert {
        "type": "solution",
        "output": {"dzn": "x = 1;\n"},
        "sections": ["dzn"],
    } in messages

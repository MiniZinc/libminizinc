from pathlib import Path
from minizinc import default_driver, Driver
import subprocess
import json


def test_multiple_type_errors():
    here = Path(__file__).resolve().parent
    assert isinstance(default_driver, Driver)
    model_file = here / "test_multiple_type_errors.mzn"
    p = subprocess.run(
        [default_driver._executable, model_file, "--json-stream"],
        stdin=None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    assert p.returncode == 1
    messages = [json.loads(l) for l in p.stdout.splitlines(False)]
    errors = [m for m in messages if m["type"] == "error"]
    assert len(errors) == 4
    assert all(e["what"] == "type error" for e in errors)
